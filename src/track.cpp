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

void Track::getFaceBox(const cv::Rect &originRect, int maxWidth, cv::Rect &faceRect) {
  auto rect = originRect;
  if (rect.y >= rect.height) {
    rect.y -= rect.height;
    rect.height *= 2;
  } else {
    rect.height += rect.y;
    rect.y = 0;
  }
  if (rect.x >= rect.width / 2) {
    rect.x -= rect.width / 2;
  } else {
    rect.x = 0;
  }
  if (rect.x + rect.width * 2 <= maxWidth) {
    rect.width *= 2;
  } else {
    rect.width = maxWidth - rect.x;
  }
  faceRect = rect;
}

void Track::doDrawWork(cv::Mat &mo, const cv::Mat &bgmask, const std::vector<ktrack::DetectInfo> &results) {
  // landmark
  for (auto &result : results) {
    cv::Mat cutMask(bgmask, getRect(result.helmet));
    std::stringstream ss;
    static int inx = 0;
    inx = (inx + 1) % 100;
    ss << "landmark" <<trackId_ << "/" 
      << "record_" << inx << ".jpg";
    cv::Mat cut(mo, getRect(result.helmet));
    cv::imwrite(ss.str().c_str(), cut);
    ss.str("");
    ss.clear();
    ss << "mask" <<trackId_ << "/" << inx << ".jpg";
    cv::imwrite(ss.str().c_str(), cutMask);
  }
  for (auto &result : results) {
    static cv::Scalar BLUE = cv::Scalar(255, 0, 0);
    static cv::Scalar GREEN = cv::Scalar(0, 255, 0);
    static cv::Scalar RED = cv::Scalar(0, 0, 255);
    cv::Scalar scalar;
    std::string category = result.helmet.category;
    LOG(INFO) << "person" << "score:" << result.helmet.score;
    if (category == "landmark") {
      scalar = GREEN;  
    } else if (category == "with") {
      scalar = BLUE;
    } else {
      scalar = RED;
    }
    std::stringstream scoreStr;
    scoreStr << result.helmet.score;
    cv::putText(mo, scoreStr.str(), cv::Point(result.person.x + 30, result.person.y),
        cv::FONT_HERSHEY_PLAIN, 5, scalar,3);
    cv::rectangle(mo, getRect(result.helmet), scalar, 2, 1);
    cv::rectangle(mo, getRect(result.person), cv::Scalar(255,255,255), 2, 1);
  }
  static int inx = 0;
  inx = (inx + 1) % 100;
  std::stringstream ss;
  ss << "cv" <<trackId_ << "/" << inx << ".jpg";
  cv::imwrite(ss.str().c_str(), mo);
  videoInfo_->updateImage(mo);
}

void Track::detectObjects(
    const cv::Mat &m,
    std::vector<ObjectDetectResult> &persons) {
  ApiWrapper<DetectServiceCvImpl> api(detectBuffers_);
  auto detectService = api.getApi();
  detectService->getDetectResult(m, persons);
}

int Track::detectPersonInfo(
    const cv::Mat &m,
    std::vector<DetectInfo> &results) {
  std::vector<ObjectDetectResult> objects;
  detectObjects(m, objects);
  TrackFilter::instance()
    .filterPersonScore(configParam_.detect.confidence, objects);
  TrackFilter::instance().filterBorder(BORDER_WIDTH, m.cols, m.rows, objects);
  TrackFilter::instance().filterPersonAspect(configParam_.detect.heightWidthThresh, objects);
  for (auto &person : objects) {
    int persony = person.y - configParam_.detect.upLength * person.height;
    if (persony < 0) {
      persony = 0;
    }
    int diff = person.y - persony;
    DetectInfo detectInfo;
    detectInfo.person.x = person.x;
    detectInfo.person.y = person.y;
    detectInfo.person.width = person.width;
    detectInfo.person.height = person.height / 2;
    detectInfo.helmet.x = person.x;
    detectInfo.helmet.y = persony;
    detectInfo.helmet.width = person.width;
    detectInfo.helmet.height = 
person.height * configParam_.detect.hatRate + diff;
    LOG(INFO) << "DIFF :" << diff;
    results.push_back(detectInfo);
  }
  return 0;
}

void Track::fineHelmetBoxByFace(
    const cv::Mat &m,
    std::vector<DetectInfo> &results) {
  for (auto & result : results) {
    if (!result.face.valid) {
      continue;
    }
    cv::Rect fineBox;
    getFaceBox(result.face.getRect(), m.cols, fineBox);
    result.helmet.setRect(fineBox);
  }
}

int Track::detectFaceInfo(
    const cv::Mat &m,
    std::vector<DetectInfo> &results) {
  if (results.empty()) {
    return 0;
  }
  ApiWrapper<FaceApi> faceApiWrapper(faceBuffers_);
  auto faceApi = faceApiWrapper.getApi();
  for (auto & result : results) {
    cv::Mat personImage(m, getRect(result.person));
    std::vector<FaceLocation> faceList;
    faceApi->getLocations(personImage, faceList);
    TrackFilter::instance().filterFaceScore(configParam_.face.confidence, faceList);
    if (faceList.size() > 0) {
      FaceLocation faceLocation = faceList[0];

      result.face.valid = true;
      setRect(faceLocation.rect(), result.face); 
      result.face.x += result.person.x;
      result.face.y += result.person.y;
    }
  }
  return 0;
}

int Track::preDetect(const cv::Mat &m,
    std::vector<DetectInfo> &results) {
  detectPersonInfo(m, results);
  detectFaceInfo(m, results);
  if (results.empty()) {
    return 0;
  }
  fineHelmetBoxByFace(m, results);
  return 0;
}


int Track::detect(const cv::Mat &m,
    std::vector<DetectInfo> &results) {
  cv::Mat bgmask;
  double rate;
  if (!moveDetect_->moveDetect(m, 
        bgmask,
        rate) || 
      rate < configParam_.detect.moveDetectRate) {
    videoInfo_->updateImage(bgmask);
    return 0;
  }
  preDetect(m, results);
  TrackFilter::instance().filterBackground(configParam_.detect.headAreaRate, bgmask, results);
  if (results.size() == 0) {
    videoInfo_->updateImage(bgmask);
    return 0;
  }

  for (auto &detectInfo : results) {
    cv::Mat personImage(m, getRect(detectInfo.helmet));
    HelmetCheckResult checkResult;
    checkResult.score = 0;
    helmetControlInfo_->checkHelmet(personImage, checkResult); 
    if (checkResult.score < configParam_.helmet.confidence) {
      LOG(INFO) << "score too little:" << checkResult.score;
      continue;
    }

    detectInfo.helmet.score = 100 *checkResult.score;
    detectInfo.helmet.category = checkResult.name;
  }
  TrackFilter::instance().filterHelmetScore(configParam_.helmet.confidence * 100, results);
  cv::Mat mo = m;
  doDrawWork(mo, bgmask, results);
  return 0;
}

} //namespace ktrack
