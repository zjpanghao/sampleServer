#ifndef INCLUDE_TRACK_TRACK_DRAW_H
#define INCLUDE_TRACK_TRACK_DRAW_H
#include <vector>
#include "opencv2/opencv.hpp"
#include "faceEntity.h"
class TrackDraw {
 public:
  static TrackDraw & instance() {
    static TrackDraw draw;
    return draw;
  }
  void doDrawWork(int trackId, const cv::Mat &bgmask, const std::vector<ktrack::DetectInfo> &results, cv::Mat &mo);

};
#endif
