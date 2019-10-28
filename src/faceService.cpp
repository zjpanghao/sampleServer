#include "faceService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "util.h"
#include "faceConst.h"
#include "pbase64/base64.h"
#include "opencv2/opencv.hpp"
namespace kface {
FaceService& FaceService::getFaceService() {
  static FaceService faceService;
  return faceService;
}

FaceService::FaceService() {
}

int FaceService::detect(int caseId, 
    const std::string &image, 
    std::vector<ktrack::ObjectDetectResult> result[]) {
  auto track = trackControl_->getTrackById(caseId);
  if (track == nullptr) {
    return -1;
  }
  std::vector<unsigned char> data;
  Base64::getBase64().decode(image, data);
  cv::Mat mo = cv::imdecode(data, cv::ImreadModes::IMREAD_COLOR);
  return track->detect(mo, result);
}
void FaceService::init(const kunyan::Config &config) {
  //std::shared_ptr<DBPool> pool(new DBPool());
  // pool->PoolInit(new DataSource(config));
  //newsRepo_.reset(new FaceRepo(pool));
  trackControl_ = std::make_shared<ktrack::TrackControl>(config);
  trackControl_->init();
  trackControl_->startTrack(0, false);
  trackControl_->startTrack(3, true);
  //track_->init(config);
  //start_ = true;
}

int FaceService::trackStart() {
  return 0;
}

int FaceService::trackStop() {
  return 0;
}

std::string FaceService::getLatestImage(int caseId) {
  auto track = trackControl_->getTrackById(caseId);
  if (track == nullptr) {
    return "";
  }
  cv::Mat m =  track->getLatestImage();
  if (m.cols == 0) {
    return "";
  }
  std::vector<unsigned char> data;
  cv::imencode(".jpg", m, data);
  std::string value;
  Base64::getBase64().encode(data, value);
  std::string image = "data:image/jpeg;base64,";
  image += value;
  return image;
}

}
