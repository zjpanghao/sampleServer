#ifndef INCLUDE_FACE_SERVICE_H
#define INCLUDE_FACE_SERVICE_H
#include <map>
#include <memory>

#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include "config/config.h"
#include "trackControl.h"
#include "faceEntity.h"
#include <list>

namespace kface {
class FaceService {
 public:
  static FaceService& getFaceService();
  FaceService();
  int detect(int caseId,
      const std::string &image, 
      std::vector<ktrack::DetectInfo> &results);

  void init(const kunyan::Config &config); 
  //std::shared_ptr<News> getLatestNews();
  std::string getLatestImage(int caseId);

 private:
  std::shared_ptr<ktrack::TrackControl> trackControl_;
};

} // namespace kface

#endif
