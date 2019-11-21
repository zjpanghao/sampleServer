#ifndef INCLUDE_FACE_ENTITY_H
#define INCLUDE_FACE_ENTITY_H
#include <opencv2/opencv.hpp>
namespace ktrack {
struct ObjectDetectResult {
  bool valid{false};
  std::string category;
  float score;
  int x;
  int y;
  int width;
  int height;

  void setRect(const cv::Rect &rect) {
    x = rect.x;
    y = rect.y;
    width = rect.width;
    height = rect.height;
  }

  cv::Rect  getRect() {
    return cv::Rect(x, 
                    y,
                    width,
                    height);
  }
};

struct DetectInfo {
  ObjectDetectResult face;
  ObjectDetectResult person;
  ObjectDetectResult helmet;
};
struct DetectParam {
  double confidence;
  double hatRate;
  double heightWidthThresh; 
  double upLength;
  double moveDetectRate;
  double headAreaRate;
};

struct FaceParam {
  double confidence;
};

struct HelmetParam {
  double confidence;
  int nums;
  bool record;
};

struct ConfigParam {
  DetectParam detect;
  FaceParam  face;
  HelmetParam helmet;
};

}
#endif
