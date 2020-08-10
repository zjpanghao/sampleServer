#include "faceRepo.h"
#include "db/sqlTemplate.h"
#include "faceMapper.h"
namespace kface {
FaceRepo::FaceRepo(std::shared_ptr<DBPool> pool):pool_(pool) {
}

std::shared_ptr<News> FaceRepo::getLatestNews(){
  std::shared_ptr<News> news;
  std::string sql = "select user_id from face_lib limit 1";
  SqlTemplate tem(pool_);
  FaceUidMapper<std::string> mapper;
  std::vector<std::string> newsList;
  tem.query(sql, mapper, newsList);
  if (!newsList.empty()) {
    news.reset(new News());
    news->id  = atoi(newsList[0].c_str());
  }
  return news;
}

}
