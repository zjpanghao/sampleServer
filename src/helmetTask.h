#ifndef INCLUDE_HELMET_TASK_H
#define INCLUDE_HELMET_TASK_H
#include "threadpool/thread_pool.h"
#include <atomic>

struct HelmetArg {
  std::atomic<int> number;
};
class HelmetTask : public Runnable {
 public:
   HelmetTask(std::shared_ptr<HelmetArg> arg);
   void Run() override;

 private:
   std::shared_ptr<HelmetArg> arg_;

};
#endif
