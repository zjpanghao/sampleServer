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

class DBPool;
namespace kface {
struct News;
class FaceRepo;
class FaceService {
 public:
  static FaceService& getFaceService();
  FaceService();
  void init(const kunyan::Config &config); 
  //std::shared_ptr<News> getLatestNews();
  std::string getLatestImage(int caseId);
  int trackStart();
  int trackStop();

 private:
  //std::shared_ptr<FaceRepo> newsRepo_;
  std::shared_ptr<ktrack::TrackControl> trackControl_;
  //std::map<int, std::shared_ptr<ktrack::Track>> tracks_;
  std::mutex lock_;
  volatile bool start_{false};
};
}

#endif
