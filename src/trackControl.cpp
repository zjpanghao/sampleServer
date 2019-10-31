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

  for (int i = 0; i < CLIENTS_NUM; i++) {
    std::stringstream ss;
    ss << "helmet" << i;
    if (config_.get(ss.str(), "server") != "") {
      LOG(INFO) <<"init helmet:" <<ss.str();
        helmetClients_[i].init(helmetNumbers,
        config_, ss.str());
    }
  }
  executorService_ = std::make_shared<ExecutorService>(helmetNumbers);
  
  std::string value =  config_.get("detect", "helmetConfidence");
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

  value =  config_.get("detect", "heightWidthThresh");
  configParam_.detect.heightWidthThresh = 3;
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam_.detect.heightWidthThresh;
  }
  value =  config_.get("detect", "upLength");
  configParam_.detect.upLength = 10;
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam_.detect.upLength;
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
  }
  // movedetect
  value =  config_.get("detect", "moveDetectRate");
  configParam_.detect.moveDetectRate = 0.003;
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam_.detect.moveDetectRate;
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
  return;
  std::string recv(buf, len);
  Json::Value root;
  Json::Reader reader;
  bool f = reader.parse(recv, root);
  if (!f) {
    return;
  }
  int  caseId = -1;
  //caseId = root["caseId"].asInt();
  JsonUtil::getJsonInt(root, "caseId", caseId);
  if (caseId == -1) {
    LOG(ERROR) << "error recv command message, no caseId found";
  }
  if (trackMp_.count(caseId) > 0) {
    return;
  }
  std::stringstream ss;
  std::shared_ptr<Track> track= std::make_shared<Track>(caseId,
    detectBuffers_, 
                                                        faceBuffers_, 
                                                        helmetClients_[caseId % CLIENTS_NUM],
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


void TrackControl::startTrack(int number,
                              bool withKafka) {
  int caseId = number % 20;
  if (trackMp_.count(caseId) != 0) {
    return;
  }
  ConfigParam configParam = configParam_;
  std::shared_ptr<Track> track= std::make_shared<Track>(caseId, 
    detectBuffers_, 
                                                        faceBuffers_, 
                                                        helmetClients_[caseId % CLIENTS_NUM],
                                                        configParam);
  std::stringstream ss;
  ss <<"detect" << caseId;
  std::string detect = ss.str();
  std::string value;
  value =  config_.get(detect, "upLength");
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam.detect.upLength;
  }

  value =  config_.get(detect, "confidence");
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam.detect.confidence;
  }

  value =  config_.get(detect, "hatRate");
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam.detect.hatRate;
  }

  value =  config_.get(detect, "heightWidthThresh");
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam.detect.heightWidthThresh;
  }
  value =  config_.get(detect, "helmetConfidence");
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam.helmet.confidence;
  }
  value =  config_.get(detect, "record");
  ss.clear();
  ss.str("");
  if (value != "") {
    ss << value;
    ss >> configParam.helmet.record;
    LOG(INFO) << "set recored" << configParam.helmet.record;
  }

  std::string server = config_.get("kafka", "server");
  
  ss <<"face_" << caseId;
  if (!withKafka) {
    server = "";
  }
  if (0 != track->init(executorService_, server, ss.str(), "group_face")) {
    LOG(ERROR) << "track start error!";
    return;
  }
  trackMp_.insert(std::make_pair(caseId, track));
  LOG(INFO) << "add track caseId:" << caseId;
}

} //namespace ktrack
