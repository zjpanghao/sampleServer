#include "faceService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "util.h"
#include "faceConst.h"
#include "faceRepo.h"
namespace kface {
FaceService& FaceService::getFaceService() {
  static FaceService faceService;
  return faceService;
}

FaceService::FaceService() {
}

void FaceService::init(const kunyan::Config &config) {
  std::shared_ptr<DBPool> pool(new DBPool());
  pool->PoolInit(new DataSource(config));
  newsRepo_.reset(new FaceRepo(pool));
}

std::shared_ptr<News> FaceService::getLatestNews() {
  return newsRepo_->getLatestNews();
}

}
