#ifndef INCLUDE_DETECT_SERVICE_H
#define INCLUDE_DETECT_SERVICE_H
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include "faceEntity.h"
#include "config/config.h"
#include <opencv2/core.hpp>
struct network;
namespace kface {
class DetectService {
 public:
  static DetectService& getDetectService();
  virtual int init(const kunyan::Config &config);
  virtual int getDetectResult(const cv::Mat &m, std::vector<ObjectDetectResult> &result);
  DetectService();
 private:
  network *net_;
  //list *options_;
  //char *name_list_;
  char **names_;

};

}

#endif
