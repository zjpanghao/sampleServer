#include "moveDetect.h"
#include <glog/logging.h>
void ObjectMoveDetect::initMoveDetect(
    int thresh,
    bool detectShadow) {
  bgsub_=cv::createBackgroundSubtractorMOG2();
  bgsub_->setVarThreshold(thresh);
  bgsub_->setDetectShadows(detectShadow);
  LOG(INFO) <<"shadow:" <<bgsub_->getDetectShadows();
}

bool ObjectMoveDetect::moveDetect(const cv::Mat &m,
    cv::Mat &bgmask,
    double &rate) {
  std::call_once(initFlag_, &ObjectMoveDetect::initMoveDetect, this, 18, false);
  int status = 0;
  bool f = status_.compare_exchange_strong(status, 1);
  if (!f)  {
    LOG(INFO) <<  "bgmode already in processing"; 
    return false;
  }
  bgsub_->apply(m, bgmask); 
  status_ = 0;
  cv::erode(bgmask, bgmask, cv::Mat());
  cv::dilate(bgmask, bgmask, cv::Mat());
  cv::medianBlur(bgmask, bgmask, 3);
  int whiteCount = 0;
  for (int i = 0; i < bgmask.rows; i++) {
    uchar *ptr = bgmask.ptr(i);
    const uchar *end = ptr + bgmask.cols;
    while (ptr < end) {
      if (*ptr > 125) {
        whiteCount++;
        *ptr = 255;
      } else {
        *ptr = 0;
      }
      ptr++;
    }
  }
  rate = (double)whiteCount / (bgmask.rows * bgmask.cols);
  //LOG(INFO) << "caseId:" <<trackId_ <<"rate is:" <<rate;;
  return true;
}

