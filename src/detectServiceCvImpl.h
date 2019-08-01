
#ifndef INCLUDE_DETECT_SERVICE_CV_IMPL_H
#define INCLUDE_DETECT_SERVICE_CV_IMPL_H
#include "detectService.h"
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
#include <opencv2/dnn.hpp>

namespace ktrack {
class DetectServiceCvImpl : public DetectService {
 public:
  virtual int init(const kunyan::Config &config) override;
  virtual int getDetectResult(const cv::Mat &m, 
                                     std::vector<ObjectDetectResult> &result) override;
 private:
  cv::dnn::Net net_;
};

} // namespace kface

#endif
