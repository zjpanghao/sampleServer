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

}
#endif
