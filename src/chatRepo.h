
#ifndef INCLUDE_CHAT_REPO_H
#define INCLUDE_CHAT_REPO_H
#include <list>
#include <memory>
#include <string>
#include "db/dbpool.h"
#include "chatEntity.h"
namespace knews {
struct Chat;
class ChatRepo {
 public:
  ChatRepo(std::shared_ptr<DBPool> pool);
  std::list<std::shared_ptr<Chat>> getChatSmallThan(int id, int size);
  int saveChat(std::shared_ptr<Chat> chat); 
  int getLatestId();
 private:
  std::shared_ptr<DBPool> pool_;
};

}
#endif
