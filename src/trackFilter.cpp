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

} // namespace ktrack
