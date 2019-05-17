
#ifndef INCLUDE_CHAT_SERVICE_H
#define INCLUDE_CHAT_SERVICE_H
#include <list>
#include <map>
#include <memory>

#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include "config/config.h"

class DBPool;
namespace knews {
struct Chat;
class ChatRepo;
class ChatService {
 public:
  static ChatService& getChatService();
  ChatService();
  void init(const kunyan::Config &config); 
  int saveChat(std::shared_ptr<Chat> chat);
  int getLatestChat();
  std::list<std::shared_ptr<Chat>> getLatestChatMore(int id, int size);

 private:
  std::shared_ptr<ChatRepo> chatRepo_;
};
}

#endif
