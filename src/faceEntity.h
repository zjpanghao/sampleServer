#ifndef INCLUDE_FACE_ENTITY_H
#define INCLUDE_FACE_ENTITY_H
#include <opencv2/opencv.hpp>
namespace ktrack {
struct ObjectDetectResult {
  std::string category;
  float score;
  int x;
  int y;
  int width;
  int height;

};

struct DetectParam {
  double confidence;
  double hatRate;
  double widthHeightThresh; 
  double heightWidthThresh; 
  int upLength;
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
