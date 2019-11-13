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
  LOG(INFO) << "trackControl exit";
}

template<class T>
void TrackControl::setConfig(
    const std::string &segment,
    const std::string &key,
    T   &param,
    T   defaultValue) {
  std::string value =  
    config_.get(segment, key);
  param = defaultValue;
  if (value != "") {
    std::stringstream ss;
    ss << value;
    ss >> param;
  }
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

  for (int i = 0; i < CLIENTS_NUM; i++) {
    std::stringstream ss;
    ss << "helmet" << i;
    if (config_.get(ss.str(), "server") != "") {
      LOG(INFO) <<"init helmet:" <<ss.str();
        helmetClients_[i].init(helmetNumbers,
        config_, ss.str());
    }
  }
  
  setConfig("detect", "helmetConfidence",
      configParam_.helmet.confidence,
      0.7);
  setConfig("detect", "heightWidthThresh",
      configParam_.detect.heightWidthThresh,
      3.0);
  setConfig("detect", "upLength",
    configParam_.detect.upLength,
    0.2);
  setConfig("detect", "hatRate",
    configParam_.detect.hatRate,
    0.2);
  setConfig("detect", "confidence",
    configParam_.detect.confidence,
    0.6);
  setConfig("detect", "record",
    configParam_.helmet.record,
    false);
  setConfig("detect", "moveDetectRate",
    configParam_.detect.moveDetectRate,
    0.003);
  setConfig("detect", "headAreaRate",
    configParam_.detect.headAreaRate,
    0.3);
  setConfig("face", "confidence",
    configParam_.face.confidence,
    0.6);
  return 0;
}

std::shared_ptr<Track> TrackControl::getTrackById(int caseId) {
  auto it = trackMp_.find(caseId);
  if (it == trackMp_.end()) {
    return nullptr;
  }
  return it->second;
}

void TrackControl::startTrack(int number) {
  int caseId = number % 20;
  if (trackMp_.count(caseId) != 0) {
    return;
  }
  ConfigParam configParam = configParam_;
  std::stringstream ss;
  ss <<"detect" << caseId;
  std::string detect = ss.str();
  std::string value;
  setConfig(detect, "hatRate",
    configParam.detect.hatRate,
    configParam_.detect.hatRate);
  setConfig(detect, "helmetConfidence",
      configParam.helmet.confidence,
      configParam_.helmet.confidence);
  LOG(INFO) << detect << " helmetConfidence:" << configParam.helmet.confidence;
  setConfig(detect, "heightWidthThresh",
      configParam.detect.heightWidthThresh,
      configParam_.detect.heightWidthThresh);
  setConfig(detect, "upLength",
    configParam.detect.upLength,
    configParam_.detect.upLength);
  setConfig(detect, "record",
    configParam.helmet.record,
    configParam_.helmet.record);
  setConfig(detect, "moveDetectRate",
    configParam.detect.moveDetectRate,
    configParam_.detect.moveDetectRate);
  setConfig(detect, "headAreaRate",
    configParam.detect.headAreaRate,
    configParam_.detect.headAreaRate);
  std::shared_ptr<Track> track= std::make_shared<Track>(caseId, 
    detectBuffers_, 
                                                        faceBuffers_, 
                                                        helmetClients_[caseId % CLIENTS_NUM],
                                                        configParam);
  if (0 != track->init()) {
    LOG(ERROR) << "track start error!";
    return;
  }
  trackMp_.insert(std::make_pair(caseId, track));
  LOG(INFO) << "add track caseId:" << caseId;
}

} //namespace ktrack
