#include "helmetTask.h"
#include <glog/logging.h>
HelmetTask::HelmetTask(std::shared_ptr<HelmetArg> arg):arg_(arg) {

}

void HelmetTask::Run() {
  LOG(INFO) << "task run";
}
