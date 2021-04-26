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

class AliMail;
class DBPool;
namespace kun {
  class Timer;
};
namespace kems {
struct News;
class EmsRepo;
class EmsService {
 public:
  ~EmsService();

  static EmsService& getEmsService();
  EmsService();
  int init(const kunyan::Config &config); 
  int sendEms(const std::string &subject,
      const std::string &body,
      const std::string &to);
  
 private:
  std::shared_ptr<EmsRepo> emsRepo_;
  std::mutex lock_;
  AliMail *mail_;
  kun::Timer *timer_;
};
}

#endif
