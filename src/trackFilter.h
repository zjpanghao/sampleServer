#ifndef INCLUDE_TRACK_FILTER_H
#define  INCLUDE_TRACK_FILTER_H
#include <vector>
#include "faceEntity.h"
namespace ktrack {
class TrackFilter {
 public:
   static TrackFilter &instance() {
      static TrackFilter filter;
      return filter;
   }  
   void filterPersonScore(double score,
                     std::vector<ObjectDetectResult> &results);
   void filterBorder(
                int border, 
                int width, 
                int height,
                std::vector<ObjectDetectResult> &results);
   void filterPersonAspect(double rate, std::vector<ObjectDetectResult> &results);
};

} // namespace 
#endif
