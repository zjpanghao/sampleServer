#include "projecttestService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "projecttestConst.h"
#include "projecttestRepo.h"
#include "resource/resource.h"
namespace kprojecttest {
ProjecttestService& ProjecttestService::getProjecttestService() {
  static ProjecttestService projecttestService;
  return projecttestService;
}

ProjecttestService::ProjecttestService() {
}

int ProjecttestService::init(const kunyan::Config &config) {
  LOG(INFO)  << "service init";
  auto pool = Resource::getResource().dbPool();
  newsRepo_.reset(new ProjecttestRepo(pool));
  return 0;
}

std::shared_ptr<News> ProjecttestService::getLatestNews() {
  return newsRepo_->getLatestNews();
}

}
