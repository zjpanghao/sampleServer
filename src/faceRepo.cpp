#include "faceRepo.h"
#include <glog/logging.h>

namespace knews {
FaceRepo::FaceRepo(std::shared_ptr<DBPool> pool):pool_(pool) {
}

int FaceRepo::saveNews(std::shared_ptr<News> news) {
  int rc = 0;
  Connection_T conn = pool_->GetConnection();
  Connection_execute(conn, "set names utf8");
  PreparedStatement_T p = Connection_prepareStatement(conn, 
      "insert into news_image(head, content_base64) values(?,?)");
  PreparedStatement_setString(p, 1, news->image.head.c_str());
  PreparedStatement_setString(p, 2, news->image.imageBase64.c_str());
  TRY {
    PreparedStatement_execute(p);
  } CATCH(SQLException) {
    LOG(ERROR) << "insert news_image error" << Exception_frame.message;
    rc = -1;
    goto END;
  }
  END_TRY;
  //LOG(INFO) << "last imageid is:" << Connection_lastRowId(conn);
  TRY { 
    p= Connection_prepareStatement(conn, 
                              " SELECT LAST_INSERT_ID()"); 
    ResultSet_T r = PreparedStatement_executeQuery(p);
    if (ResultSet_next(r)) {
      news->image.imageId = ResultSet_getInt(r, 1);
    } else {
      rc = -3;
      goto END;
    }
  } CATCH(SQLException) {
    LOG(ERROR) << "select last insert id  error" << Exception_frame.message;
    rc = -2;
    goto END;
  }
  END_TRY;
  p = Connection_prepareStatement(conn, 
      "insert into news(abstract, image_id) values(?, ?)");
  PreparedStatement_setString(p, 1, news->abstract.c_str() == NULL ? "null" : news->abstract.c_str());
  PreparedStatement_setInt(p, 2, news->image.imageId);
  TRY {
    PreparedStatement_execute(p);
  } CATCH(SQLException) {
    rc = -4;
    LOG(ERROR) << "insert news error:" << Exception_frame.message;
    goto END;
  }
  END_TRY;
END:
  pool_->returnConnection(conn);
  return rc;
}

std::shared_ptr<News> FaceRepo::getLatestNews() {
  std::shared_ptr<News> news;
#ifdef USE_MYSQL
  Connection_T conn = pool_->GetConnection();
  Connection_execute(conn, "set names utf8");
  PreparedStatement_T p = Connection_prepareStatement(conn, 
                              "SELECT a.id,  a.abstract, b.head, b.content_base64  FROM news a inner join news_image b on a.image_id = b.image_id  order by a.id desc limit 1"); 
    ResultSet_T r = NULL;

  TRY {
    r = PreparedStatement_executeQuery(p);
  } CATCH(SQLException) {
    LOG(ERROR) << "search latest news error:" << Exception_frame.message;
    goto END;
  }
  END_TRY;
  if (ResultSet_next(r)) {
    news.reset(new News());
    news->id = ResultSet_getInt(r, 1);
    //news->url = ResultSet_getString(r, 2);
    news->abstract = ResultSet_getString(r, 2);
    news->image.head = ResultSet_getString(r, 3);
    news->image.imageBase64 = ResultSet_getString(r, 4);
  }
END:
  pool_->returnConnection(conn);
#endif
  return news;
}

std::list<std::shared_ptr<News>> FaceRepo::getNews(int page, int size) {
  std::list<std::shared_ptr<News>> newsList;
  std::shared_ptr<News> news;
#ifdef USE_MYSQL
  Connection_T conn = pool_->GetConnection();
  Connection_execute(conn, "set names utf8");
  PreparedStatement_T p = Connection_prepareStatement(conn, 
                              "SELECT a.id,  a.abstract, b.content_base64  FROM news a inner join news_image b on a.image_id = b.image_id  order by a.id desc limit ?,?"); 
  PreparedStatement_setInt(p, 1, page * size);
  PreparedStatement_setInt(p, 2, size);
  ResultSet_T r = PreparedStatement_executeQuery(p);
  if (ResultSet_next(r)) {
    news.reset(new News());
    news->id = ResultSet_getInt(r, 1);
    //news->url = ResultSet_getString(r, 2);
    news->abstract = ResultSet_getString(r, 2);
    news->image.imageBase64 = ResultSet_getString(r, 3);
    newsList.push_back(news);
  }
  pool_->returnConnection(conn);
#endif
  return newsList;
}

std::list<std::shared_ptr<News>> FaceRepo::getNewsSmallThan(int id, int size) {
  std::list<std::shared_ptr<News>> newsList;
#ifdef USE_MYSQL
  Connection_T conn = pool_->GetConnection();
  Connection_execute(conn, "set names utf8");
  PreparedStatement_T p = Connection_prepareStatement(conn, 
                              "SELECT a.id,  a.abstract, b.head,  b.content_base64  FROM news a inner join news_image b on a.image_id = b.image_id  where a.id < ? order by a.id desc limit ?"); 
  PreparedStatement_setInt(p, 1, id);
  PreparedStatement_setInt(p, 2, size);
  ResultSet_T r;
  TRY {
    r = PreparedStatement_executeQuery(p);
  } CATCH(SQLException) {
    LOG(ERROR) << "get news error:" << Exception_frame.message;
    goto END;
  }
  END_TRY;
  while (ResultSet_next(r)) {
    std::shared_ptr<News> news;
    news.reset(new News());
    news->id = ResultSet_getInt(r, 1);
    //news->url = ResultSet_getString(r, 2);
    news->abstract = ResultSet_getString(r, 2);
    news->image.head = ResultSet_getString(r, 3);
    news->image.imageBase64 = ResultSet_getString(r, 4);
    newsList.push_back(news);
  }
#endif
END:
  pool_->returnConnection(conn);
  return newsList;
}

std::list<std::shared_ptr<News>> FaceRepo::getNewsInfoSmallThan(int id, int size) {
  std::list<std::shared_ptr<News>> newsList;
#ifdef USE_MYSQL
  Connection_T conn = pool_->GetConnection();
  Connection_execute(conn, "set names utf8");
  PreparedStatement_T p = Connection_prepareStatement(conn, 
                              "SELECT a.id,  a.abstract, b.image_id, b.head  FROM news a inner join news_image b on a.image_id = b.image_id  where a.id < ? order by a.id desc limit ?"); 
  PreparedStatement_setInt(p, 1, id);
  PreparedStatement_setInt(p, 2, size);
  ResultSet_T r;
  TRY {
    r = PreparedStatement_executeQuery(p);
  } CATCH(SQLException) {
    LOG(ERROR) << "get news error:" << Exception_frame.message;
    goto END;
  }
  END_TRY;
  while (ResultSet_next(r)) {
    std::shared_ptr<News> news;
    news.reset(new News());
    news->id = ResultSet_getInt(r, 1);
    //news->url = ResultSet_getString(r, 2);
    news->abstract = ResultSet_getString(r, 2);
    news->image.imageId = ResultSet_getInt(r, 3);
    news->image.head = ResultSet_getString(r, 4);
    newsList.push_back(news);
  }
#endif
END:
  pool_->returnConnection(conn);
  return newsList;
}

int FaceRepo::getImage(int imageId, std::string &image) {
  int rc = -1;
#ifdef USE_MYSQL
  Connection_T conn = pool_->GetConnection();
  PreparedStatement_T p = Connection_prepareStatement(conn, 
                              "SELECT content_base64 from news_image where image_id = ?"); 
  PreparedStatement_setInt(p, 1, imageId);
  ResultSet_T r;
  TRY {
    r = PreparedStatement_executeQuery(p);
  } CATCH(SQLException) {
    LOG(ERROR) << "get Image error:" << Exception_frame.message;
    rc = -2;
    goto END;
  }
  END_TRY;
  if (ResultSet_next(r)) {
    image = ResultSet_getString(r, 1);
    rc = 0;
  }
END:
  pool_->returnConnection(conn);
#endif
  return rc;  
}

} // namespace knews
