#include "emsService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "emsConst.h"
#include "emsRepo.h"
#include "resource/resource.h"
#include "email/aliMail.h"
#include "email/aliConn.h"
#include "timer/timer.h"
namespace kems {
EmsService::~EmsService() {
  delete mail_;
}

EmsService& EmsService::getEmsService() {
    static EmsService emsService;
  return emsService;
}

EmsService::EmsService() {
}

int EmsService::init(const kunyan::Config &config) {
  LOG(INFO)  << "service init";
  auto pool = Resource::getResource().dbPool();
  emsRepo_.reset(new EmsRepo(pool));
  mail_ = new AliMail(config.get("user", "name"),
      config.get("user", "password"));
  
  timer_ = &kun::Timer::getTimer();
  timer_->addFunc(30, 0, [&] {
    auto news = emsRepo_->getLatestNews();
    if (news == nullptr) {
      return;
    }
  LOG(INFO) << "get news" << news->id << news->title << news->stamp;
  std::lock_guard<std::mutex> guard(lock_);
  if (0 == mail_->sendMail(news->title, 
      news->abstract, 
      news->to)) {
    emsRepo_->updateStatus(news->id);
  }
  });
}

int EmsService::sendEms(const std::string &subject,
    const std::string &body,
    const std::string &to) {
  LOG(INFO) << "subject:" << subject;
  if (subject == "" || body == "" || to == "") {
    return -1;
  }
  News news;
  news.to = to;
  news.abstract = body;
  news.title = subject;
  news.stamp = time(NULL);
  news.priority = 0;
  emsRepo_->saveNews(news);
 // std::lock_guard<std::mutex> guard(lock_);
 // return mail_->sendMail(subject, body, to);
}

}
