#include "trackFilter.h"
#include <glog/logging.h>
namespace ktrack {
void TrackFilter::filterPersonScore(
    double score,
    std::vector<ObjectDetectResult> &results) {
  auto it = results.begin();
  while (it != results.end()) {
    if (it->category != "person" 
        || it->score < score ) {
      it =results.erase(it);
    } else {
      it++;
    }
  }
}

void TrackFilter::filterFaceAspect(
    double rate,
    std::vector<FaceLocation> &results) {
  auto it = results.begin();
  while (it != results.end()) {
    cv::Rect rect = it->rect();
    if (rect.width *rate  < rect.height ) {
      it =results.erase(it);
    } else {
      it++;
    }
  }
}

void TrackFilter::filterFaceScore(
    double score,
    std::vector<FaceLocation> &results) {
  auto it = results.begin();
  while (it != results.end()) {
    if (it->confidence()  < score ) {
      it =results.erase(it);
    } else {
      it++;
    }
  }
}

void TrackFilter::filterBorder(
    int border, 
    int width, 
    int height,
    std::vector<ObjectDetectResult> &results) {
  auto it = results.begin();
  while (it != results.end()) {
    auto &object = *it;
    if (object.x < border
        || object.x > width - border
        || object.y > height - border 
        || object.y < border) {
      it = results.erase(it);
    } else {
      it++;
    }
  }
}

void TrackFilter::filterPersonAspect(double rate, std::vector<ObjectDetectResult> &results) {
  auto it = results.begin();
  while (it != results.end()) {
    if (it->height < it->width * rate) {
      LOG(INFO) << "person widthRate too high:" << (double)it->width/it->height;
      it =results.erase(it);
    } else {
      it++;
    }
  }
}

double TrackFilter::getWhiteRate(const cv::Mat &m) {
  int count = 0;
  for (int i = 0; i < m.rows; i++) {
    const uchar *p = m.ptr(i);
    const uchar *q = p + m.cols;
    for (;p < q; p++) {
      if (*p > 0) {
        count++;
      }
    }
  }
  return (double)count / (m.rows *m.cols);
}

void TrackFilter::filterBackground(double rate, const cv::Mat &mask,std::vector<DetectInfo> &results) {
  auto it = results.begin();
  while (it != results.end()) {
    cv::Mat helmetBg(mask,it->helmet.getRect());
    double whiteRate;
    if ((whiteRate =
          getWhiteRate(helmetBg)) < rate) {
      it = results.erase(it);
    } else {
      it++;
    }
  }
}

void TrackFilter::filterCenterBackground(double rate, const cv::Mat &mask,std::vector<DetectInfo> &results) {
  auto it = results.begin();
  while (it != results.end()) {
    cv::Rect left(it->helmet.getRect());
    left.width /= 2;
    cv::Rect right(it->helmet.getRect());
    right.x += (right.width / 2);
    right.width /= 2;
    cv::Mat leftBg(mask,left);
    cv::Mat rightBg(mask,right);
    double whiteRate[2];
    whiteRate[0] = getWhiteRate(leftBg);
    whiteRate[1] = getWhiteRate(rightBg);
    if (whiteRate[0] > whiteRate[1]) {
      std::swap(whiteRate[0], whiteRate[1]);
    }
    LOG(INFO) <<"whiteRate:" << whiteRate[0] << "," << whiteRate[1]; 
    if (whiteRate[0] *rate < whiteRate[1]){
      it = results.erase(it);
    } else {
      it++;
    }
  }
}

void TrackFilter::filterHelmetScore(double score,std::vector<DetectInfo> &results) {
  auto it = results.begin();
  while (it != results.end()) {
    if (it->helmet.score < score) {
      it = results.erase(it);
    } else {
      it++;
    }
  }
}

} // namespace ktrack
