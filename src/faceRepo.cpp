#include "faceRepo.h"
namespace kface {
FaceRepo::FaceRepo(std::shared_ptr<DBPool> pool):pool_(pool) {
}

std::shared_ptr<News> FaceRepo::getLatestNews() {
  std::shared_ptr<News> news;
  Connection_T conn = pool_->GetConnection();
  Connection_execute(conn, "set names utf8");
  PreparedStatement_T p = Connection_prepareStatement(conn, 
                              "SELECT * FROM news order by id desc limit 1"); 
  ResultSet_T r = PreparedStatement_executeQuery(p);
  if (ResultSet_next(r)) {
    news.reset(new News());
    news->id = ResultSet_getInt(r, 1);
    news->url = ResultSet_getString(r, 2);
    news->abstract = ResultSet_getString(r, 3);
  }
  pool_->returnConnection(conn);
  return news;
}

}
