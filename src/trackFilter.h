#ifndef INCLUDE_TRACK_FILTER_H
#define  INCLUDE_TRACK_FILTER_H
#include <vector>
#include "faceEntity.h"
#include "faceApi.h"
#include "opencv2/opencv.hpp"
namespace ktrack {
class TrackFilter {
 public:
   static TrackFilter &instance() {
      static TrackFilter filter;
      return filter;
   }  
   void filterPersonScore(double score,
                     std::vector<ObjectDetectResult> &results);
   void filterFaceScore(double score,
                        std::vector<FaceLocation> &results);
   void filterFaceAspect(double rate,
                        std::vector<FaceLocation> &results);
   int filterBorder(
                int border, 
                int width, 
                int height,
                std::vector<ObjectDetectResult> &results);
   int filterPersonAspect(double rate, std::vector<ObjectDetectResult> &results);
   int filterBackground(double rate, const cv::Mat &mask,  std::vector<DetectInfo> &results);
   int filterCenterBackground(double rate, const cv::Mat &mask,  std::vector<DetectInfo> &results);
   void filterHelmetScore(double score, std::vector<DetectInfo> &results);
  double getWhiteRate(const cv::Mat &m);
};

} // namespace 
#endif
