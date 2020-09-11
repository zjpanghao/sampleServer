#include "projecttestRepo.h"
#include "db/sqlTemplate.h"
#include "projecttestMapper.h"
namespace kprojecttest {
ProjecttestRepo::ProjecttestRepo(std::shared_ptr<DBPool> pool):pool_(pool) {
}

std::shared_ptr<News> ProjecttestRepo::getLatestNews(){
  std::shared_ptr<News> news;
  std::string sql = "select user_id from projecttest_lib limit 1";
  SqlTemplate tem(pool_);
  ProjecttestUidMapper<std::string> mapper;
  std::vector<std::string> newsList;
  tem.query(sql, mapper, newsList);
  if (!newsList.empty()) {
    news.reset(new News());
    news->id  = atoi(newsList[0].c_str());
  }
  return news;
}

}
