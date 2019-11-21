#ifndef INCLUDE_HELMET_TASK_H
#define INCLUDE_HELMET_TASK_H
#include <atomic>
#include <opencv2/opencv.hpp>
#include "Helmet.h"
#include "helmetClient.h"
#include "threadpool/thread_pool.h"
#include "apipool/apiPool.h"
#include "videoInfo.h"
#include <glog/logging.h>
#include "helmetEntity.h"
#include "faceEntity.h"
struct HelmetCheckInfo {
  cv::Rect rect;
  cv::Rect personBox;
  cv::Mat personImage;
  ktrack::DetectInfo detectInfo;
  HelmetCheckResult result;
};

class HelmetControlInfo {
 public:
  typedef std::vector<std::shared_ptr<HelmetCheckInfo>>  CheckInfoList;

   class Cvcolor {
     public:
       static Cvcolor &color() {
         static Cvcolor cvcolor;
         return cvcolor;
       }
       cv::Scalar  RED{0, 0, 255};
       cv::Scalar  GREEN{0, 255, 0};
       cv::Scalar BLUE{255, 0, 0};
       cv::Scalar WHITE{255, 255, 255};
       cv::Scalar colors[4] {GREEN, RED, BLUE, WHITE};
     private:
       Cvcolor(){}
   };

  HelmetControlInfo(std::shared_ptr<HelmetMatData> matData,
                    ApiBuffer<HelmetClientDelegation> &clients,
                    std::shared_ptr<VideoInfo> videoCb);

  void waitDone() {
    std::unique_lock<std::mutex>  ulock(lock);
    cond.wait(ulock, [this] { return done;});
    done = false;
  }
  void doDrawWork();

  void run(int inx);

  int checkHelmet(const cv::Mat &detectImage, HelmetCheckResult &result); 

  void ErrorMsg(int code, const std::string &msg);

  void setImage(cv::Mat &m) {m_ = m;}

  void setCheckInfoList(std::vector<std::shared_ptr<HelmetCheckInfo>> checkInfoList) {
    checkInfoList_ = checkInfoList;
    number_ = checkInfoList_.size();
  }

  void setConfidence(double confidence) {
    confidence_ = confidence;
  }

 private:
  ApiBuffer<HelmetClientDelegation> &clients_;
  std::shared_ptr<VideoInfo> videoCb_;
  std::vector<std::shared_ptr<HelmetCheckInfo>> checkInfoList_;
  std::shared_ptr<HelmetMatData> matData_;
  std::mutex lock;
  bool done{false};
  std::atomic<int> number_;
  cv::Mat m_;
  std::condition_variable cond;
  double confidence_{0.7};
};

struct HelmetArg {
  std::shared_ptr<HelmetControlInfo> info;
  int personIndex;
};

class HelmetTask : public Runnable {
 public:
   HelmetTask(std::shared_ptr<HelmetArg> arg);
   void Run() override;
   void ErrorMsg(int code, const std::string &msg) override;
   std::shared_ptr<HelmetArg> arg_;
};
#endif
