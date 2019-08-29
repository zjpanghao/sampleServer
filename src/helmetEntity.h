#ifndef INCLUDE_HELMET_ENTITY_H
#define INCLUDE_HELMET_ENTITY_H
#include <opencv2/opencv.hpp>
struct HelmetMatData {
  cv::Mat error[2];
  cv::Mat right[2];
};
#endif
