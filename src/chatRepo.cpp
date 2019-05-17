
#include "chatRepo.h"
#include <glog/logging.h>

namespace knews {
ChatRepo::ChatRepo(std::shared_ptr<DBPool> pool):pool_(pool) {
}

int ChatRepo::getLatestId() {
  int count = 0;
  Connection_T conn = pool_->GetConnection();
  if (conn == NULL) {
    return -1;
  }
  ResultSet_T r;
  TRY {
    PreparedStatement_T p = Connection_prepareStatement(conn, 
        "SELECT max(id) from chat"); 
    r = PreparedStatement_executeQuery(p);
  } CATCH(SQLException) {
    LOG(ERROR) << "get chat error:" << Exception_frame.message;
    count = -1;
    goto END;
  }
  END_TRY;
  if (ResultSet_next(r)) {
    count = ResultSet_getInt(r, 1);
  }
END:
  pool_->returnConnection(conn);
  return count;
}

int ChatRepo::saveChat(std::shared_ptr<Chat> chat) {
  int rc = 0;
  Connection_T conn = pool_->GetConnection();
  if (conn == NULL) {
    return -1;
  }
  Connection_execute(conn, "set names utf8");
  PreparedStatement_T p = Connection_prepareStatement(conn, 
      "insert into chat(message) values(?)");
  PreparedStatement_setString(p, 1, chat->message.c_str());
  TRY {
    PreparedStatement_execute(p);
  } CATCH(SQLException) {
    LOG(ERROR) << "insert chat error" << Exception_frame.message;
    rc = -1;
    goto END;
  }
  END_TRY;
END:
  pool_->returnConnection(conn);
  return rc;
}

std::list<std::shared_ptr<Chat>> ChatRepo::getChatSmallThan(int id, int size) {
  std::list<std::shared_ptr<Chat>> chatList;
#ifdef USE_MYSQL
  Connection_T conn = pool_->GetConnection();
  if (conn == NULL) {
    return chatList;
  }
  Connection_execute(conn, "set names utf8");
  ResultSet_T r;
  std::stringstream ss;
  TRY {
    PreparedStatement_T p = Connection_prepareStatement(conn, 
        "SELECT a.id,  a.create_time, a.message FROM chat a where a.id < ? order by a.id desc limit ?"); 
    PreparedStatement_setInt(p, 1, id);
    PreparedStatement_setInt(p, 2, size);
    r = PreparedStatement_executeQuery(p);
  } CATCH(SQLException) {
    LOG(ERROR) << "get chat error:" << Exception_frame.message;
    goto END;
  }
  END_TRY;
  while (ResultSet_next(r)) {
    std::shared_ptr<Chat> chat;
    chat.reset(new Chat());
    chat->id = ResultSet_getInt(r, 1);
    ss.clear();
    ss.str("");
    //ss << ResultSet_getInt(r, 2);
    //ss >> chat->createTime;
    chat->createTime = ResultSet_getString(r, 2);
    chat->message = ResultSet_getString(r, 3);
    chatList.push_back(chat);
  }
#endif
END:
  pool_->returnConnection(conn);
  return chatList;
}

} // namespace knews
