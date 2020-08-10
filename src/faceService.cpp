#include "faceService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "faceConst.h"
#include "faceRepo.h"
#include "resource/resource.h"
namespace kface {
FaceService& FaceService::getFaceService() {
  static FaceService faceService;
  return faceService;
}

FaceService::FaceService() {
}

int FaceService::init(const kunyan::Config &config) {
  LOG(INFO)  << "service init";
  auto pool = Resource::getResource().dbPool();
  newsRepo_.reset(new FaceRepo(pool));
  return 0;
}

std::shared_ptr<News> FaceService::getLatestNews() {
  return newsRepo_->getLatestNews();
}

}
