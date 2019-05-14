#ifndef INCLUDE_FACE_SERVICE_H
#define INCLUDE_FACE_SERVICE_H
#include <list>
#include <map>
#include <memory>

#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include "config/config.h"

class DBPool;
class RedisPool;
namespace kface {
struct News;
class FaceRepo;
class FaceService {
 public:
  static FaceService& getFaceService();
  FaceService();
  void init(const kunyan::Config &config); 
  int saveNews(std::shared_ptr<News> news);
  std::shared_ptr<News> getLatestNews();
  std::list<std::shared_ptr<News>> getLatestNewsMore(int id, int size);

 private:
  std::shared_ptr<FaceRepo> newsRepo_;
  RedisPool *redisPool_;
};
}

#endif
