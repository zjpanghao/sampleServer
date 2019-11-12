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
#include "moveDetect.h"
#include "trackFilter.h"

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
}

int Track::init() {
  moveDetect_ = std::make_shared<ObjectMoveDetect>();
  helmetControlInfo_ = std::make_shared<HelmetControlInfo>(nullptr, clients_,videoInfo_);
  helmetControlInfo_->setConfidence(configParam_.helmet.confidence);
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
        LOG(INFO) << "person widthRate too high:" << (double)it->width/it->height;
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
      rect.height = rect.height *2;
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

double Track::getWhiteRate(const cv::Mat &m) {
  int count = 0;
  for (int i = 0; i < m.rows; i++) {
    const uchar *p = m.ptr(i);
    const uchar *q = p + m.cols;
    for (;p < q; p++) {
      if (*p > 0) {
        count++;
      }
    }
  }
  return (double)count / (m.rows *m.cols);

}

void Track::doDrawWork(cv::Mat &mo, const cv::Mat &bgmask, const std::vector<ObjectDetectResult> results[]) {
  for (int i = 0; i < results[0].size(); i++) {
    cv::Rect rect(results[1][i].x, results[1][i].y,
        results[1][i].width, results[1][i].height);
    cv::Mat cutMask(bgmask, rect);
    std::stringstream ss;
    static int inx = 0;
    inx = (inx + 1) % 100;
    ss << "landmark" <<trackId_ << "/" 
      << "record_" << inx << ".jpg";
    cv::Mat cut(mo, rect);
    cv::imwrite(ss.str().c_str(), cut);
    ss.str("");
    ss.clear();
    ss << "mask" <<trackId_ << "/" << inx << ".jpg";
    cv::imwrite(ss.str().c_str(), cutMask);
  }
  for (int i = 0; i < results[0].size(); i++) {
    static cv::Scalar BLUE = cv::Scalar(255, 0, 0);
    static cv::Scalar GREEN = cv::Scalar(0, 255, 0);
    static cv::Scalar RED = cv::Scalar(0, 0, 255);
    cv::Scalar scalar;
    std::string category = results[0][i].category;
    LOG(INFO) << "person" << i << "score:" << results[0][i].score;
    if (category == "landmark") {
      scalar = GREEN;  
    } else if (category == "with") {
      scalar = BLUE;
    } else {
      scalar = RED;
    }
    std::stringstream scoreStr;
    scoreStr << results[0][i].score;
    cv::putText(mo, scoreStr.str(), cv::Point(results[1][i].x + 30, results[1][i].y),
        cv::FONT_HERSHEY_PLAIN, 5, scalar,3);
    cv::Rect rect(results[1][i].x, results[1][i].y,
        results[1][i].width, results[1][i].height);
    cv::Rect rect2(results[0][i].x, results[0][i].y,
        results[0][i].width, results[0][i].height);
    cv::rectangle(mo, rect, scalar, 2, 1);
    cv::rectangle(mo, rect2, cv::Scalar(255,255,255), 2, 1);
  }
  static int inx = 0;
  inx = (inx + 1) % 100;
  std::stringstream ss;
  ss << "cv" <<trackId_ << "/" << inx << ".jpg";
  cv::imwrite(ss.str().c_str(), mo);
  videoInfo_->updateImage(mo);
}

void Track::detectPersons(
    const cv::Mat &m,
    std::vector<ObjectDetectResult> &persons) {
  ApiWrapper<DetectServiceCvImpl> api(detectBuffers_);
  auto detectService = api.getApi();
  detectService->getDetectResult(m, persons);
  TrackFilter::instance()
    .filterPersonScore(configParam_.detect.confidence, persons);
}

int Track::detect(const cv::Mat &m,
      std::vector<ObjectDetectResult> result[]) {
  cv::Mat bgmask;
  double rate;
  if (!moveDetect_->moveDetect(m, 
        bgmask,
        rate) || 
      rate < configParam_.detect.moveDetectRate) {
    videoInfo_->updateImage(bgmask);
    return 0;
  }
  std::vector<ObjectDetectResult> objects;
  detectPersons(m, objects);
  TrackFilter::instance().filterBorder(BORDER_WIDTH, m.cols, m.rows, objects);
  TrackFilter::instance().filterPersonAspect(configParam_.detect.heightWidthThresh, objects);
  int count = objects.size();
  if (count <= 0) {
    return 0;
  }
  LOG(INFO) << trackId_ << " origin find " << objects.size() << "person";
 
  std::vector<std::shared_ptr<HelmetCheckInfo>> checkInfos;
  for (auto &person : objects) {
    int persony = person.y - configParam_.detect.upLength * person.height;
    if (persony < 0) {
      persony = 0;
    }
    int diff = person.y - persony;
    LOG(INFO) << "DIFF :" << diff;
    cv::Rect box(person.x, persony, person.width, person.height * configParam_.detect.hatRate + diff);
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
      cv::Mat tmp(bgmask, checkInfos[personIndex]->rect);
      double whiteRate;
      if ((whiteRate = getWhiteRate(tmp)) < \
          configParam_.detect.headAreaRate) {
    std::stringstream ss;
    static int einx = 0;
    einx = (einx + 1) % 10;
    ss << "emask" <<trackId_ << "/" << einx << ".jpg";
    cv::imwrite(ss.str().c_str(), tmp);
        
        LOG(INFO) << "CUT  TOO HEIGHT ESCAPSE area :" <<  whiteRate;
          ;
        continue;
    }
    helmetControlInfo_->checkHelmet(checkInfos[personIndex]->personImage, checkResult); 
   if (checkResult.score < configParam_.helmet.confidence) {
     LOG(INFO) << "score too little:" << checkResult.score;
     continue;
   }
   auto info = checkInfos[personIndex];
   detectResult.score = 100 *checkResult.score;
   personResult.x = info->personBox.x;
   personResult.y = info->personBox.y;
   personResult.width = info->personBox.width;
   personResult.height = info->personBox.height;
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
  cv::Mat mo = m;
  doDrawWork(mo, bgmask, result);
  return 0;
}

} //namespace ktrack
