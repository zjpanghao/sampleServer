#ifndef INCLUDE_HELMET_TASK_H
#define INCLUDE_HELMET_TASK_H
#include <atomic>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include "Helmet.h"
#include "helmetClient.h"
#include "threadpool/thread_pool.h"
#include "apipool/apiPool.h"
#include "videoInfo.h"
#include <glog/logging.h>
struct HelmetCheckInfo {
  cv::Rect rect;
  cv::Mat personImage;
  HelmetCheckResult result;
};

class HelmetControlInfo {
 public:
  typedef std::vector<std::shared_ptr<HelmetCheckInfo>>  CheckInfoList;

  HelmetControlInfo(const CheckInfoList &checkInfoList);
  void waitDone() {
    std::unique_lock<std::mutex>  ulock(lock);
    cond.wait(ulock, [this] { return done;});
  }
  cv::Mat m;
  cv::Mat error[2];
  cv::Mat right[2];
  std::vector<std::shared_ptr<HelmetCheckInfo>> checkInfoList_;
  ApiBuffer<HelmetClientDelegation> *clients;
  std::shared_ptr<VideoInfo> videoCb;
  std::mutex lock;
  bool done{false};
  std::atomic<int> number_;
  std::condition_variable cond;
};

struct HelmetArg {
  std::shared_ptr<HelmetControlInfo> info;
  int personIndex;
};

class HelmetTask : public Runnable {
 public:
   HelmetTask(std::shared_ptr<HelmetArg> arg);
   int checkHelmet(const cv::Mat &detectImage, HelmetCheckResult &result); 
   void Run() override;
   std::shared_ptr<HelmetArg> arg_;
 private:
   void doDrawWork();
};
#endif
