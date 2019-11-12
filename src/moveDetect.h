#ifndef INCLUDE_TRACK_MOVE_DETECT_H
#define INCLUDE_TRACK_MOVE_DETECT_H
#include <opencv2/video/background_segm.hpp>
#include <opencv2/opencv.hpp>
#include <atomic>
#include <mutex>
class ObjectMoveDetect {
 public:
  bool moveDetect(const cv::Mat &m,
   cv::Mat &bgmask,
    double &rate);
  void initMoveDetect(int thresh,
                      bool detectShadow);

 private:
  cv::Ptr<cv::BackgroundSubtractorMOG2> bgsub_;
  std::atomic<int> status_{0};
  std::once_flag initFlag_;
};
#endif
