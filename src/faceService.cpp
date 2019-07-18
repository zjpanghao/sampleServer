#include "faceService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "util.h"
#include "faceConst.h"
#include "faceRepo.h"
#include "pbase64/base64.h"
#include "opencv2/opencv.hpp"
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
  track_ = std::make_shared<ktrack::Track>();
  track_->init(config);
  start_ = true;
}

int FaceService::trackStart() {
#if 0
  if (start_) {
    return -1;
  }
  track_ = std::make_shared<ktrack::Track>();
  //track_->init();
  start_ = true;
#endif
}

int FaceService::trackStop() {
  start_ = false;
  track_ = nullptr;
  return 0;
}

std::shared_ptr<News> FaceService::getLatestNews() {
  return newsRepo_->getLatestNews();
}

std::string FaceService::getLatestImage() {
  if (!start_) {
    return "";
  }

  cv::Mat m =  track_->getLatestImage();
  if (m.cols == 0) {
    return "";
  }
  std::vector<unsigned char> data;
  cv::imencode(".jpg", m, data);
  std::string value;
  Base64::getBase64().encode(data, value);
  std::string image = "data:image/jpeg;base64,";
  image += value;
  return image;
}

}
