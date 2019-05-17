#include "faceService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "util.h"
#include "faceConst.h"
#include "faceRepo.h"
#include "predis/redis_pool.h"
namespace knews {
FaceService& FaceService::getFaceService() {
  static FaceService faceService;
  return faceService;
}

FaceService::FaceService() {
}

void FaceService::init(const kunyan::Config &config) {
#ifdef USE_MYSQL
  std::shared_ptr<DBPool> pool(new DBPool());
  pool->PoolInit(new DataSource(config));
  newsRepo_.reset(new FaceRepo(pool));
  redisPool_ = getRedisPool();
#endif
}

int FaceService::saveNews(std::shared_ptr<News> news) {
  return newsRepo_->saveNews(news);
}

std::shared_ptr<News> FaceService::getLatestNews() {
  return newsRepo_->getLatestNews();
}

std::list<std::shared_ptr<News>> FaceService::getLatestNewsMore(int id, int size) {
  
  RedisControlGuard guard(redisPool_);
  auto control = guard.GetControl();
  if (!control) {
    return newsRepo_->getNewsSmallThan(id, size);
  }


  auto newsInfoList = newsRepo_->getNewsInfoSmallThan(id, size);
  std::stringstream ss;
  struct timeval tv[2];
  for (auto &newsInfo : newsInfoList) {
    ss.clear();
    ss.str("");
    ss << newsInfo->image.imageId;
    gettimeofday(&tv[0], NULL);
    std::string imageId;
    ss >> imageId;
    control->GetValue(imageId, &newsInfo->image.imageBase64);
    newsInfo->image.imageBase64 = "";
    gettimeofday(&tv[1], NULL);
    if (newsInfo->image.imageBase64.length() == 0) {
      LOG(INFO) << "load from db";
      newsRepo_->getImage(newsInfo->image.imageId, newsInfo->image.imageBase64);
      control->SetExValue(imageId, 1200,  newsInfo->image.imageBase64);
      //sleep(1);
    }
  }
  LOG(INFO) << tv[0].tv_sec << ":" << tv[0].tv_usec;
  LOG(INFO) << tv[1].tv_sec << ":" << tv[1].tv_usec;
  return newsInfoList;
}

}
