#include "track.h"
#include <opencv2/opencv.hpp>
#include "pbase64/base64.h"
#include <json/json.h>
#include <glog/logging.h>
#include "faceEntity.h"
#include "helmetEntity.h"
#include "detectService.h"
#include "detectServiceCvImpl.h"
#include "helmetTask.h"

using  apache::thrift::protocol::TProtocol;
using  apache::thrift::protocol::TBinaryProtocol;
using  apache::thrift::transport::TTransport;
using  apache::thrift::transport::TSocket;
using  apache::thrift::transport::TTransportException;
using  apache::thrift::transport::TBufferedTransport;

#define BORDER_WIDTH 150
namespace ktrack {
Track::Track(int trackId, ApiBuffer<DetectServiceCvImpl> &apiBuffers,
    ApiBuffer<FaceApi> &faceBuffers, 
    ApiBuffer<HelmetClientDelegation> &clients, 
    const ConfigParam &configParam) 
    : trackId_(trackId), detectBuffers_(apiBuffers), faceBuffers_(faceBuffers), clients_(clients), videoInfo_(std::make_shared<VideoInfo>()), configParam_(configParam){
}

Track::~Track() {
  stop();
  LOG(INFO) << "track task exit";
}

int Track::init(std::shared_ptr<ExecutorService> executorService,
    const std::string &kafkaServer,
    const std::string &topic,
    const std::string &group
    ) {
  executorService_ = executorService;
  topic_ = topic;
if (kafkaServer != "") {
  int rc = Init(kafkaServer, topic, group);
  if (rc == 0) {
    rc |= StartAll();
  }
  if (rc != 0) {
    return -1;
  }
}
  index_ = 0;
  matData_ = std::make_shared<HelmetMatData>();
  matData_->right[0] = cv::imread("right.jpg");
  matData_->error[0] = cv::imread("error.jpg");
  matData_->right[1] = cv::imread("right.jpg", 0);
  matData_->error[1] = cv::imread("error.jpg", 0);
  helmetControlInfo_ = std::make_shared<HelmetControlInfo>(matData_, clients_,videoInfo_);
  helmetControlInfo_->setConfidence(configParam_.helmet.confidence);
  bgsub_=cv::createBackgroundSubtractorMOG2();
  bgsub_->setVarThreshold(18);
  return 0;
}

cv::Mat Track::getLatestImage() {
  return videoInfo_->getImage();
}

void Track::filterPersons(std::vector<ObjectDetectResult> &persons,
                                int maxWidth, int maxHeight) {
  auto it = persons.begin();
  while (it != persons.end()) {
    if (it->category != "person" || it->score < configParam_.detect.confidence || !rectValid(*it,
    maxWidth, maxHeight)) {
      it =persons.erase(it);
    } else {
      if (it->height < it->width * configParam_.detect.heightWidthThresh) {
        it =persons.erase(it);
        continue;
      }
      it++;
    }
  }
}

bool Track::getHelmetBox(std::vector<FaceLocation> &faces, int maxWidth, int maxHeight, cv::Rect &rect) {
  auto it = faces.begin();
  while (it != faces.end()) {
    if (it->confidence() > configParam_.face.confidence) {
      //auto &location = it->second;
      rect = it->rect();
      rect.x -= rect.width / 2;
      rect.y -= rect.height;
      if (rect.x < 0) {
        rect.x = 0;
      }
      if (rect.y < 0) {
        rect.y = 0;
      }
      if (rect.y + rect.height >= maxHeight) {
        rect.height = maxHeight - rect.y;
      }

      rect.width  = rect.width * 2;
      if (rect.width + rect.x >= maxWidth) {
        rect.width = maxWidth - rect.x;
      }
      return true;
    } 
    it++;
  }
  return false;
}

bool Track::rectValid(const ObjectDetectResult &object, int maxWidth, int maxHeight) {
  if (object.x < BORDER_WIDTH) {
    return false;
  }

  if (object.x > maxWidth - BORDER_WIDTH) {
    return false;
  }

  if (object.y > maxHeight - BORDER_WIDTH || object.y < 0) {
    return false;
  }

  if (object.width  + object.x + BORDER_WIDTH> maxWidth) {
    return false;
  }

  if (object.height > maxHeight - object.y) {
    return false;
  }
  return true;
}

void Track::ProcessMessage(const char *buf, int len) {
  //LOG(INFO) << "recv len:" << len << " tid:" << std::this_thread::get_id();
  std::string recv(buf, len);
  Json::Value root;
  Json::Reader reader;
  bool f = reader.parse(recv, root);
  if (!f) {
    return;
  }
  std::string image = root["image"].asString();
  std::string dateTime = root["dateTime"].asString();
  //LOG(INFO) << "dateTime:" << dateTime;
  std::vector<unsigned char> data;
  Base64::getBase64().decode(image, data);
  cv::Mat mo = cv::imdecode(data, cv::ImreadModes::IMREAD_COLOR);
  //cv::Mat m(mo, cv::Rect(0, 0, mo.cols * 1 / 3 + 200 , mo.rows));
  cv::Mat m = mo;//.clone();
  cv::Mat bgmask;
  if (moveDetect(mo, bgmask)) {
    videoInfo_->updateImage(bgmask);
    return;
  }
  std::vector<ObjectDetectResult> objects;
  {
    ApiWrapper<DetectServiceCvImpl> api(detectBuffers_);
    auto detectService = api.getApi();
    detectService->getDetectResult(m, objects);
  }
  filterPersons(objects, m.cols, m.rows);
  int count = objects.size();
  // no person found
  if (count <= 0) {
    videoInfo_->updateImage(m);
    return;
  }
 
  std::vector<std::shared_ptr<HelmetCheckInfo>> checkInfos;
  //cv::Mat showImage = count > 1 ? m.clone() : m;
  for (auto &person : objects) {
    person.y = person.y > configParam_.detect.upLength ? person.y - configParam_.detect.upLength : 0;
    cv::Rect box(person.x, person.y, person.width, person.height * configParam_.detect.hatRate);
    LOG(INFO) << "hatrate:" << configParam_.detect.hatRate;
    cv::rectangle(m, box, cv::Scalar(255,255,255));
    imwrite("test.jpg", m);
    cv::Rect faceDetectBox(box);
    cv::Mat faceImage(m, faceDetectBox);
    std::vector<FaceLocation> faceList;
    // check if face found
    {
      ApiWrapper<FaceApi> faceApiWrapper(faceBuffers_);
      auto faceApi = faceApiWrapper.getApi();
      faceApi->getLocations(faceImage, faceList);
    }
    auto boxDetect = box;
    cv::Rect helmetBox;
    bool valid = getHelmetBox(faceList, faceImage.cols, faceImage.rows, helmetBox);
    if (valid) {
      box = helmetBox;
      faceImage = cv::Mat(faceImage, helmetBox);
      box.x  += boxDetect.x;
      box.y  += boxDetect.y;
    }
    if (configParam_.helmet.record) {
      std::stringstream file1;
      file1 << "landmark/" << "landmark_" << index_ << ".jpg";
      index_ = (index_ + 1) % 1000;
      cv::imwrite(file1.str(), faceImage);
    }
    std::shared_ptr<HelmetCheckInfo> checkInfo 
      = std::make_shared<HelmetCheckInfo>();
    checkInfo->rect = box;
    checkInfo->personImage = faceImage;
    checkInfos.push_back(checkInfo);
  }
  
  helmetControlInfo_->setImage(m);
  helmetControlInfo_->setCheckInfoList(checkInfos);
  for (int personIndex = 0; personIndex < checkInfos.size(); personIndex++) {
    std::shared_ptr<HelmetArg> arg = std::make_shared<HelmetArg>(); 
    arg->personIndex = personIndex;
    arg->info = helmetControlInfo_;
    std::unique_ptr<HelmetTask> helmetTask (new HelmetTask(arg));
    executorService_->Execute(std::move(helmetTask));
  }
  auto start = std::chrono::steady_clock::now();
  helmetControlInfo_->waitDone();
  auto end = std::chrono::steady_clock::now();
  LOG(INFO) << topic_ <<  " faces:" << checkInfos.size() 
    << " dure:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int Track::detect(const cv::Mat &m,
      std::vector<ObjectDetectResult> result[]) {
  cv::Mat bgmask;
  if (!moveDetect(m, bgmask)) {
    return 0;
  }
  std::vector<ObjectDetectResult> objects;
  {
    ApiWrapper<DetectServiceCvImpl> api(detectBuffers_);
    auto detectService = api.getApi();
    detectService->getDetectResult(m, objects);
  }
  filterPersons(objects, m.cols, m.rows);
  int count = objects.size();
  if (count <= 0) {
    return 0;
  }
 
  std::vector<std::shared_ptr<HelmetCheckInfo>> checkInfos;
  for (auto &person : objects) {
    int originy = person.y;
    person.y = person.y > configParam_.detect.upLength ? person.y - configParam_.detect.upLength : 0;
    int upLength = originy - person.y;
    cv::Rect box(person.x, person.y, person.width, person.height * configParam_.detect.hatRate - upLength);
    cv::Rect faceDetectBox(box);
    cv::Mat faceImage(m, faceDetectBox);
    std::vector<FaceLocation> faceList;
    {
      ApiWrapper<FaceApi> faceApiWrapper(faceBuffers_);
      auto faceApi = faceApiWrapper.getApi();
      faceApi->getLocations(faceImage, faceList);
    }
    auto boxDetect = box;
    cv::Rect helmetBox;
    bool valid = getHelmetBox(faceList, faceImage.cols, faceImage.rows, helmetBox);
    if (valid) {
      box = helmetBox;
      faceImage = cv::Mat(faceImage, helmetBox);
      box.x  += boxDetect.x;
      box.y  += boxDetect.y;
    }
    std::shared_ptr<HelmetCheckInfo> checkInfo 
      = std::make_shared<HelmetCheckInfo>();
    checkInfo->rect = box;
    checkInfo->personBox = cv::Rect(person.x, person.y, person.width, person.height / 2); 
    checkInfo->personImage = faceImage;
    checkInfos.push_back(checkInfo);
  }
  LOG(INFO) << checkInfos.size();
  HelmetCheckResult checkResult;
  ObjectDetectResult detectResult;
  ObjectDetectResult personResult;
  for (int personIndex = 0; personIndex < checkInfos.size(); personIndex++) {
    helmetControlInfo_->checkHelmet(checkInfos[personIndex]->personImage, checkResult); 
   if (checkResult.score < configParam_.helmet.confidence) {
     continue;
   }
   detectResult.score = 100 *checkResult.score;
   personResult.x = objects[personIndex].x;
   personResult.y = objects[personIndex].y;
   personResult.width = objects[personIndex].width;
   personResult.height = objects[personIndex].height;
   personResult.score = objects[personIndex].score * 100;
   detectResult.x = 
     checkInfos[personIndex]->rect.x;
   detectResult.y = checkInfos[personIndex]->rect.y;
   detectResult.width = checkInfos[personIndex]->rect.width;
   detectResult.height = checkInfos[personIndex]->rect.height;
   detectResult.category = checkResult.name;
   std::swap(detectResult.x, personResult.x);
   std::swap(detectResult.y, personResult.y);
   std::swap(detectResult.height, personResult.height);
   std::swap(detectResult.width, personResult.width);
   result[0].push_back(detectResult); 
   result[1].push_back(personResult); 
  }
  return 0;
}

bool Track::moveDetect(const cv::Mat &m,
    cv::Mat &bgmask) {
  bgsub_->apply(m, bgmask); 
  cv::erode(bgmask, bgmask, cv::Mat());
  double rate = 0.0;
  int whiteCount = 0;
  for (int i = 0; i < bgmask.rows; i++) {
    const uchar *ptr = bgmask.ptr(i);
    const uchar *end = ptr + bgmask.cols;
    while (ptr < end) {
      if (*ptr > 0) {
        whiteCount++;
      }
      ptr++;
    }
  }
  rate = (double)whiteCount / (bgmask.rows * bgmask.cols);
  LOG(INFO) << "caseId:" <<trackId_ <<"rate is:" <<rate;;
  return rate > configParam_.detect.moveDetectRate;
}

} //namespace ktrack
