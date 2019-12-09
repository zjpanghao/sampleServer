#include "track.h"
#include <chrono>
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
#include "trackDraw.h"

using  apache::thrift::protocol::TProtocol;
using  apache::thrift::protocol::TBinaryProtocol;
using  apache::thrift::transport::TTransport;
using  apache::thrift::transport::TSocket;
using  apache::thrift::transport::TTransportException;
using  apache::thrift::transport::TBufferedTransport;

#define BORDER_WIDTH 100
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

void Track::getLatestImage(std::string &image) {
  videoInfo_->getImage(image);
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

void Track::detectObjects(
    const cv::Mat &m,
    std::vector<ObjectDetectResult> &persons) {
  ApiWrapper<DetectServiceCvImpl> api(detectBuffers_);
  auto detectService = api.getApi();
  auto start = std::chrono::steady_clock::now();
  detectService->getDetectResult(m, persons);
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> diff = end - start;
  for (auto &obj : persons) {
    obj.costTime = diff.count();
  }
}

int Track::detectPersonInfo(
    const cv::Mat &m,
    std::vector<DetectInfo> &results) {
  std::vector<ObjectDetectResult> objects;
  detectObjects(m, objects);
  TrackFilter::instance()
    .filterPersonScore(configParam_.detect.confidence, objects);
  int cnt = TrackFilter::instance().filterBorder(BORDER_WIDTH, m.cols, m.rows, objects);
  if (cnt > 0) {
    LOG(INFO) << "border filter cnt:" << cnt;
  }
  cnt = TrackFilter::instance().filterPersonAspect(configParam_.detect.heightWidthThresh, objects);
  if (cnt > 0) {
    LOG(INFO) << "aspect filter cnt:" << cnt;
  }
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
    detectInfo.person.costTime = 
      person.costTime;
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

void Track::fineHelmetBoxHorizontalByBG(
    int horizontalThresh,
    const cv::Mat &bgmask,
    std::vector<DetectInfo> &results) {
  for (auto & result : results) {
    int fineLeft = 0; 
    int fineRight = 0; 
    cv::Mat helmetImage(bgmask, 
        result.helmet.getRect());
    int thresh = horizontalThresh > helmetImage.rows ? helmetImage.rows : horizontalThresh;
    for (int j = 0; j < helmetImage.cols; j++) {
      int count = 0;
      for (int i = 0; i < helmetImage.rows; i++) {
        uchar *p = helmetImage.ptr<uchar>(i);
        count += *(p + j) > 0;
        if (count >= thresh) {
          break;
        }
      }

      if (count >= thresh) {
        fineLeft = j;
        break;
      }
    } 
    fineLeft -= 10;
    if (fineLeft < 0) {
      fineLeft = 0;
    }
    for (int j = helmetImage.cols -1; j >= 0; j--) {
      int count = 0;
      for (int i = 0; i < helmetImage.rows; i++) {
        uchar *p = helmetImage.ptr<uchar>(i);
        count += *(p + j) > 0;
        if (count >= thresh) {
          break;
        }
      }

      if (count >= thresh) {
        fineRight = j;
        break;
      }
    } 
    fineRight += 10;
    if (fineRight >= result.helmet.width) {
      fineRight = result.helmet.width -1;
    }
    if (fineRight > fineLeft) {
      result.helmet.x += fineLeft;
      result.helmet.width = fineRight - fineLeft + 1;
    }
  }
}

void Track::fineHelmetBoxByBackground(
    int widthThresh,
    const cv::Mat &bgmask,
    std::vector<DetectInfo> &results) {
  for (auto & result : results) {
    int refiney = 0; 
    cv::Mat helmetImage(bgmask, 
      result.helmet.getRect());
    for (int i = 0; i < helmetImage.rows; i++) {
      uchar *p = helmetImage.ptr<uchar>(i);
      int count = 0;
      int thresh = widthThresh > helmetImage.cols ? helmetImage.cols : widthThresh;
      for (int j = 0; j < helmetImage.cols; j++) {
        count += *p++ > 0;
        if (count >= thresh) {
          break;
        }
      }

      if (count >= thresh) {
        refiney = i;
        break;
      }
    } 
    refiney -= 10;
    if (refiney > 0) {
      result.helmet.y += refiney;
      result.helmet.height -= refiney;
    }
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
    auto start = std::chrono::steady_clock::now();
    faceApi->getLocations(personImage, faceList);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    result.face.costTime = diff.count();
    TrackFilter::instance().filterFaceScore(configParam_.face.confidence, faceList);
    TrackFilter::instance().filterFaceAspect(2.2, faceList);
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
    const cv::Mat &mask,
    std::vector<DetectInfo> &results) {
  detectPersonInfo(m, results);

  detectFaceInfo(m, results);
  if (results.empty()) {
    return 0;
  }
  fineHelmetBoxByFace(m, results);
  fineHelmetBoxByBackground(10, mask, results);
  fineHelmetBoxHorizontalByBG(10, mask, results);
  int cnt = TrackFilter::instance().filterBackground(configParam_.detect.headAreaRate, mask, results);
  if (cnt > 0) {
    LOG(INFO) << "headArea filter size:" <<     cnt;
  }
  cnt = TrackFilter::instance().filterCenterBackground(2, mask, results);
  if (cnt > 0) {
  LOG(INFO) << " center bg filter size:" <<     cnt;
  }
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
    //LOG(INFO) << "rate:" << rate;
    videoInfo_->updateImage(bgmask);
    return 0;
  }
  preDetect(m, bgmask, results);
  if (results.size() == 0) {
    videoInfo_->updateImage(m);
    return 0;
  }

  for (auto &detectInfo : results) {
    cv::Mat personImage(m, getRect(detectInfo.helmet));
    HelmetCheckResult checkResult;
    checkResult.score = 0;
    auto start = std::chrono::steady_clock::now();
    helmetControlInfo_->checkHelmet(personImage, checkResult); 
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    detectInfo.helmet.costTime = diff.count();
    detectInfo.helmet.score = 100 *checkResult.score;
    detectInfo.helmet.category = checkResult.name;
  }
  TrackFilter::instance().filterHelmetScore(configParam_.helmet.confidence * 100, results);

  if (!results.empty()) {
    cv::Mat mo = m;
    TrackDraw::instance().doDrawWork(
        trackId_,
        bgmask, 
        results,
        mo);
  }
  videoInfo_->updateImage(m);
  return 0;
}

} //namespace ktrack
