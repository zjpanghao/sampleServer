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
namespace ktrack {
class DetectService {
 public:
  virtual int init(const kunyan::Config &config) = 0;
  virtual int getDetectResult(const cv::Mat &m, 
                                     std::vector<ObjectDetectResult> &result) = 0;
  virtual ~DetectService() = default;
 private:
};

}

#endif
