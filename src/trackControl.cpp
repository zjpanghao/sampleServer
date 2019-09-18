#include "trackControl.h"
#include <opencv2/opencv.hpp>
#include "pbase64/base64.h"
#include <json/json.h>
#include <glog/logging.h>
#include "faceEntity.h"
#include "detectService.h"
#include "detectServiceCvImpl.h"
#include "util.h"
#include "Helmet.h"

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

  int helmetNumbers = 1;
  ss.clear();
  ss.str("");
  ss << config_.get("helmet", "num");
  ss >> helmetNumbers;


  helmetClients_.init(helmetNumbers, config_);
  executorService_ = std::make_shared<ExecutorService>(helmetNumbers);
  
  std::string value =  config_.get("helmet", "confidence");
  configParam_.helmet.confidence = 0.8;
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam_.helmet.confidence;
  }

  value =  config_.get("detect", "widthHeightThresh");
  configParam_.detect.widthHeightThresh = 3;
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam_.detect.widthHeightThresh;
  }

  value =  config_.get("detect", "hatRate");
  configParam_.detect.hatRate = 0.2;
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam_.detect.hatRate;
  }

  value =  config_.get("detect", "confidence");
  configParam_.detect.confidence = 0.6;
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam_.detect.confidence;
  }

  value =  config_.get("detect", "record");
  configParam_.helmet.record = false;
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam_.helmet.record;
    LOG(INFO) << "set recored" << configParam_.helmet.record;
  }

  value =  config_.get("face", "confidence");
  configParam_.face.confidence = 0.6;
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam_.face.confidence;
  }
  
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
  if (trackMp_.count(caseId) > 0) {
    return;
  }
  std::stringstream ss;
  std::shared_ptr<Track> track= std::make_shared<Track>(detectBuffers_, 
                                                        faceBuffers_, 
                                                        helmetClients_,
                                                        configParam_);
  std::string server = config_.get("kafka", "server");
  ss.clear();
  ss.str("");
  ss << "face_" << caseId;
  if (0 != track->init(executorService_, server, ss.str(), "group_face")) {
    LOG(ERROR) << "track start error!";
    return;
  }
  trackMp_.insert(std::make_pair(caseId, track));
  LOG(INFO) << "add track caseId:" << caseId;
}

} //namespace ktrack
