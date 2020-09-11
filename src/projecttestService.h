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

class DBPool;
namespace kprojecttest {
struct News;
class ProjecttestRepo;
class ProjecttestService {
 public:
  static ProjecttestService& getProjecttestService();
  ProjecttestService();
  int init(const kunyan::Config &config); 
  std::shared_ptr<News> getLatestNews();

 private:
  std::shared_ptr<ProjecttestRepo> newsRepo_;
};
}

#endif
