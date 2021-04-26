#include "emsRepo.h"
#include "db/sqlTemplate.h"
#include "emsMapper.h"
namespace kems {
EmsRepo::EmsRepo(std::shared_ptr<DBPool> pool):pool_(pool) {
}

std::shared_ptr<News> EmsRepo::getLatestNews(){
  std::shared_ptr<News> news;
  std::string sql = "select id, `to`, title, abstract, stamp  \
                     from ems_news where status = 0 order by stamp desc limit 1";
  SqlTemplate tem(pool_);
  EmsMapper<News> mapper;
  std::vector<News> newsList;
  tem.query(sql, mapper, newsList);
  if (!newsList.empty()) {
    news.reset(new News(newsList[0]));
  }
  return news;
}

int EmsRepo::updateStatus(int id) {
  SqlTemplate newsTemp(pool_);
  PreparedStmt pst("update ems_news set status = 1 where id = ?");
  pst.setInt(1, id);
  newsTemp.update(pst);
  return 0;
}

int EmsRepo::saveNews(const News &news) {
  SqlTemplate newsTemp(pool_);
  PreparedStmt pst("insert into ems_news(`to`,title,abstract)\
      values (?,?,?)");
  pst.setString(1, news.to);
  pst.setString(2, news.title);
  pst.setString(3, news.abstract);
//  pst.setInt(4, news.stamp);
  newsTemp.update(pst);
  return 0;
}

}
