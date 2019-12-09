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

int TrackFilter::filterBorder(
    int border, 
    int width, 
    int height,
    std::vector<ObjectDetectResult> &results) {
  auto it = results.begin();
  int cnt = 0;
  while (it != results.end()) {
    auto &object = *it;
    if (object.x < border
        || object.x + object.width > width - border
        || object.y + object.height > height - border 
        || object.y < border) {
      it = results.erase(it);
      cnt++;
    } else {
      it++;
    }
  }
  return cnt;
}

int TrackFilter::filterPersonAspect(double rate, std::vector<ObjectDetectResult> &results) {
  auto it = results.begin();
  int cnt = 0;
  while (it != results.end()) {
    if (it->height < it->width * rate) {
      LOG(INFO) << "person aspect too high:" << (double)it->width/it->height;
      it =results.erase(it);
      cnt++;
    } else {
      it++;
    }
  }
  return cnt;
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

int TrackFilter::filterBackground(double rate, const cv::Mat &mask,std::vector<DetectInfo> &results) {
  auto it = results.begin();
  int cnt = 0;
  while (it != results.end()) {
    cv::Mat helmetBg(mask,it->helmet.getRect());
    double whiteRate;
    if ((whiteRate =
          getWhiteRate(helmetBg)) < rate) {
      it = results.erase(it);
      cnt++;
    } else {
      it++;
    }
  }
  return cnt;
}

int TrackFilter::filterCenterBackground(double rate, const cv::Mat &mask,std::vector<DetectInfo> &results) {
  int cnt = 0;
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
    if (whiteRate[0] *rate < whiteRate[1]){
      it = results.erase(it);
      cnt++;
      LOG(INFO) <<"whiteRate:" << whiteRate[0] << "," << whiteRate[1]; 
    } else {
      it++;
    }
  }
  return cnt;
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
