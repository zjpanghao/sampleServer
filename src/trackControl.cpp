#include "trackControl.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include "pbase64/base64.h"
#include <json/json.h>
#include <glog/logging.h>
#include "faceEntity.h"
#include "detectService.h"
#include "detectServiceCvImpl.h"
#include "util.h"

namespace ktrack {
TrackControl::TrackControl(const kunyan::Config &config) : config_(config) {
}

TrackControl::~TrackControl() {
  stop();
  LOG(INFO) << "trackControl exit";
}

int TrackControl::init() {
  std::stringstream ss;
  int detectNums = DETECT_BUFFER_NUM;
  int faceNums = FACE_BUFFER_NUM;
  
  ss << config_.get("detectors", "num");
  ss >> detectNums;
  ss.clear();
  ss.str("");
  ss << config_.get("face", "num");
  ss >> faceNums;
  detectBuffers_.init(detectNums, config_);
  faceBuffers_.init(faceNums);
  int rc = Init(config_.get("kafka", "server"),
                config_.get("kafka", "topic"), 
                config_.get("kafka", "group")
              );
  if (rc == 0) {
    rc |= StartAll();
  }
  if (rc != 0) {
    return -1;
  }
  return 0;
}

std::shared_ptr<Track> TrackControl::getTrackById(int caseId) {
  std::lock_guard<std::mutex> guard(lock_);
  auto it = trackMp_.find(caseId);
  if (it == trackMp_.end()) {
    return nullptr;
  }
  return it->second;
}

void TrackControl::ProcessMessage(const char *buf, int len) {
  std::string recv(buf, len);
  Json::Value root;
  Json::Reader reader;
  bool f = reader.parse(recv, root);
  if (!f) {
    return;
  }
  int  caseId = -1;
  getJsonInt(root, "caseId", caseId);
  if (caseId == -1) {
    LOG(ERROR) << "error recv command message, no caseId found";
  }
  if (trackMp_.count(caseId) == 0) {
    return;
  }
  std::shared_ptr<Track> track= std::make_shared<Track>(detectBuffers_, faceBuffers_);
  std::string server = config_.get("kafka", "server");
  std::stringstream ss;
  ss << "face_" << caseId;
  if (0 != track->init(server, ss.str(), "group_face")) {
    LOG(ERROR) << "track start error!";
    return;
  }
  trackMp_.insert(std::make_pair(caseId, track));
  LOG(INFO) << "add track caseId:" << caseId;
}

} //namespace ktrack
