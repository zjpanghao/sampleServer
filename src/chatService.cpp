
#include "chatService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "util.h"
#include "chatRepo.h"
#include "predis/redis_pool.h"
namespace knews {
ChatService& ChatService::getChatService() {
  static ChatService chatService;
  return chatService;
}

ChatService::ChatService() {
}

void ChatService::init(const kunyan::Config &config) {
#ifdef USE_MYSQL
  std::shared_ptr<DBPool> pool(new DBPool());
  pool->PoolInit(new DataSource(config));
  chatRepo_.reset(new ChatRepo(pool));
#endif
}

int ChatService::saveChat(std::shared_ptr<Chat> chat) {
  return chatRepo_->saveChat(chat);
}

int ChatService::getLatestChat() {
  return chatRepo_->getLatestId();
}

std::list<std::shared_ptr<Chat>> ChatService::getLatestChatMore(int id, int size) {
    return chatRepo_->getChatSmallThan(id, size);
}

}
