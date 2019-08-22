#include "track.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include "pbase64/base64.h"
#include <json/json.h>
#include <glog/logging.h>
#include "faceEntity.h"
#include "detectService.h"
#include "detectServiceCvImpl.h"

using  apache::thrift::protocol::TProtocol;
using  apache::thrift::protocol::TBinaryProtocol;
using  apache::thrift::transport::TTransport;
using  apache::thrift::transport::TSocket;
using  apache::thrift::transport::TTransportException;
using  apache::thrift::transport::TBufferedTransport;

#define PERSON_CONF  0.5
#define BORDER_WIDTH 150
namespace ktrack {
Track::Track(ApiBuffer<DetectServiceCvImpl> &apiBuffers,
    ApiBuffer<FaceApi> &faceBuffers) 
    : detectBuffers_(apiBuffers), faceBuffers_(faceBuffers) {
}

Track::~Track() {
  if (client_ != nullptr) {
    client_->getInputProtocol()->getTransport()->close();
  }
  stop();
  LOG(INFO) << "track task exit";
}

bool Track::initClient() {
  std::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
  std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  client_ = std::make_shared<HelmetClient>(protocol);
  try {
    transport->open();
  } catch (TTransportException &e) {
    LOG(ERROR) << e.what();
    return false;
  }
  return true;
}

int Track::init(std::shared_ptr<ExecutorService> executorService,
                const std::string &kafkaServer,
                  const std::string &topic,
                  const std::string &group
                  ) {
  executorService_ = executorService;
  int rc = Init(kafkaServer, topic, group);
  if (rc == 0) {
    rc |= StartAll();
  }
  if (rc != 0) {
    return -1;
  }
  index_ = 0;
  right_[0] = cv::imread("right.jpg");
  error_[0] = cv::imread("error.jpg");
  right_[1] = cv::imread("right.jpg", 0);
  error_[1] = cv::imread("error.jpg", 0);
  if (!initClient()) {
    LOG(ERROR) << "rpc init failed";
    return -1;
  }
  return 0;
}

cv::Mat Track::getLatestImage() {
  std::lock_guard<std::mutex> lck(lock_);
  return image_;
}

static bool errorRc(int rc) {
  return rc == 1 ? false : true;
}

void Track::filterPersons(std::vector<ObjectDetectResult> &persons,
                                int maxWidth, int maxHeight) {
  auto it = persons.begin();
  while (it != persons.end()) {
    if (it->category != "person" || it->score < PERSON_CONF || !rectValid(*it,
    maxWidth, maxHeight)) {
      it =persons.erase(it);
    } else {
      it++;
    }
  }
}

bool Track::getHelmetBox(std::vector<FaceLocation> &faces, int maxWidth, int maxHeight, cv::Rect &rect) {
  auto it = faces.begin();
  while (it != faces.end()) {
    if (it->confidence() > 0.5) {
      //auto &location = it->second;
      rect = it->rect();
      rect.x -= rect.width / 2;
      if (rect.x < 0) {
        rect.x = 0;
      }
      rect.y -= rect.height / 2;
      if (rect.y < 0) {
        rect.y = 0;
      }
      rect.height +=  rect.height / 2;
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

  if (object.width  + object.x > maxWidth) {
    return false;
  }

  if (object.height > maxHeight - object.y) {
    return false;
  }
  return true;
}

int Track::checkHelmet(const cv::Mat &detectImage, HelmetCheckResult &result) {
  std::vector<unsigned char> faceImageData;
  cv::imencode(".jpg", detectImage, faceImageData);
  std::string imageBase64;
  Base64::getBase64().encode(faceImageData, imageBase64);
  bool status = true;
  int rc = -1;
  try {
    if (errorConnectCount_ >= 1 && time(NULL) > errorTime_ + 3) {
      status = initClient();
      errorConnectCount_ = 0;
      errorTime_ = time(NULL);
    }
    if (status) {
      client_->checkHelmet(result, imageBase64);
      rc = 0;
    }
    errorConnectCount_ = 0;
    } catch (::apache::thrift::transport::TTransportException &e) {
      LOG(ERROR) << "transport exception:" << e.what();
      errorConnectCount_++;
      if (errorConnectCount_ == 1) {
        errorTime_ = time(NULL);
      }
    } catch (::apache::thrift::TApplicationException &e) {
      LOG(ERROR) << "check helemt exception" << e.what();
      rc = -1;
    } catch (std::exception &e) {
      LOG(ERROR) << "check helemt std::exception" << e.what();
      rc = -2;
    } 
    return rc;
}

void Track::ProcessMessage(const char *buf, int len) {
  LOG(INFO) << "recv len:" << len << " tid:" << std::this_thread::get_id();
  std::string recv(buf, len);
  Json::Value root;
  Json::Reader reader;
  bool f = reader.parse(recv, root);
  if (!f) {
    return;
  }
  std::string image = root["image"].asString();
  std::string dateTime = root["dateTime"].asString();
  LOG(INFO) << "dateTime:" << dateTime;
  std::vector<unsigned char> data;
  Base64::getBase64().decode(image, data);
  cv::Mat mo = cv::imdecode(data, cv::ImreadModes::IMREAD_COLOR);
  //cv::Mat m(mo, cv::Rect(mo.cols / 3, 0, mo.cols * 2 / 3 , mo.rows));
  cv::Mat m = mo;//.clone();
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
    std::lock_guard<std::mutex> lck(lock_);
    image_ = m;
    return;
  }
  
  cv::Mat showImage = count > 1 ? m.clone() : m;
  for (auto &person : objects) {
    LOG(INFO) << "(" << person.x <<"," << person.y << "," 
    << person.width << "," << person.height <<"," << person.score;
    cv::Rect box(person.x, person.y, person.width, person.height/6);
    cv::Rect faceDetectBox(person.x, person.y, person.width, person.height/2);
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
#if DEBUG
    std::stringstream file1;
    file1 << "landmark/" << "landmark_" << index_ << ".jpg";
    index_ = (index_ + 1) % 1000;
    if (faceImage.cols < faceImage.rows * 1.7) {
      cv::imwrite(file1.str(), faceImage);
    }
#endif

    HelmetCheckResult checkResult;
    int ret = checkHelmet(faceImage, checkResult);
    LOG(INFO) << "checkhelmet:" << ret;
    static cv::Scalar  RED(0, 0, 255);
    static cv::Scalar  GREEN(0, 255, 0);
    static cv::Scalar BLUE(255, 0, 0);
    static cv::Scalar WHITE(255, 255, 255);
    cv::Scalar *colors[] = {&RED, &GREEN, &BLUE, &WHITE};
    cv::Scalar  &scalar = BLUE;
    
    if (ret == 0 && checkResult.errorCode == 0) {
      int rc = checkResult.index;
      scalar = *colors[rc];
      cv::Mat &alert = (errorRc(rc)) ? error_[0] : right_[0];
      cv::Mat &alertMask = (errorRc(rc)) ? error_[1] : right_[1];
      cv::Rect alertBox(box.x, box.y - alert.rows < 0 ? 0 : box.y - alert.rows, 
          alert.cols,  alert.rows);
      cv::Mat alertImage(showImage, alertBox);
      alert.copyTo(alertImage, alertMask);
      cv::rectangle(showImage, box, scalar, 2, 1);
    }
  }
  
  std::stringstream file;
  file << "cv/" << index_ << ".jpg";
  index_ = (index_ + 1) % 1000;
  cv::imwrite(file.str(), showImage);
  m = showImage;
   
  std::lock_guard<std::mutex> lck(lock_);
  image_ = m;
}

} //namespace ktrack
