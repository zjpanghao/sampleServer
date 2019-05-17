
#include "chatControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

#ifdef EVENT__HAVE_NETINET_IN_H
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#endif
#include "event2/http_compat.h"
#include "json/json.h"
#include "event2/http.h"


#include <thread>
#include <vector>
#include <glog/logging.h>
#include <iterator>
#include <regex>
#include "util.h"
#include "httpUtil.h"

#include "chatService.h"
#include "chatEntity.h"
#include <pbase64/base64.h>

namespace knews {
void ChatControl::chatDetectCb(http_request *req, void *arg) {
  struct timeval tv[2];
  int rc = 0;
  Json::Value root;
  Json::Reader reader;
  //evbuffer *response = req->buffer_out;
  evbuffer *response = evbuffer_new();
#if 0
  if (req->method != htp_method_POST) {
    rc = -1;
    sendResponse(rc, "method not support", req, response);
    return;
  }
#endif
  
  std::string body = getBodyStr(req);
  if (!reader.parse(body, root)) {
    rc = -3;
    sendResponse(rc, "parse error", req, response);
    return;
  }
  
  int chatId = 0;
  int size = 2;
  JsonUtil::getJsonInt(root, "chatId", chatId);
  JsonUtil::getJsonInt(root, "size", size);
 
  if (chatId == 0) {
    rc = -4;
    sendResponse(rc, "chatId error", req, response);
    return;
  }
  LOG(INFO) << chatId << "," << size;
  //getJsonInt(root, "max_chat_num", chatNum);
  //std::string decodeData;
  //int decodeLen = 0;
  //std::vector<unsigned char> cdata;
  //cdata.assign(&decodeData[0], &decodeData[0] + decodeLen);
  gettimeofday(&tv[0], NULL);
  ChatService &service = ChatService::getChatService(); 
  std::list<std::shared_ptr<Chat>> chatList = service.getLatestChatMore(chatId, size);
  gettimeofday(&tv[1], NULL);
  LOG(INFO) << tv[0].tv_sec << ":" << tv[0].tv_usec;
  LOG(INFO) << tv[1].tv_sec << ":" << tv[1].tv_usec;
  LOG(INFO) << "get chat size:" << chatList.size();
  Json::Value items;
  for (auto &chat : chatList) { 
    Json::Value item;
    item["chatId"] = chat->id;
    item["createTime"] =  chat->createTime;
    item["message"] =  chat->message;
    items.append(item);
  }
  Json::Value res;
  res["items"] = items;
  evbuffer_add_printf(response, "%s", res.toStyledString().c_str());
  evhttp_send_reply(req, 200, "ss", response);
  //evhtp_send_reply(req, EVHTP_RES_OK);
}

void ChatControl::chatUploadChatCb(http_request *req, void *arg) {
  struct timeval tv[2];
  int rc = 0;
  Json::Value root;
  Json::Value chatResult;
  Json::Value items;
  Json::Reader reader;
 
  //evbuffer *response = req->buffer_out;
  evbuffer *response = evbuffer_new();
#if 0
  if (req->method != htp_method_POST) {
    rc = -1;
    sendResponse(rc, "method not support", req, response);
    return;
  }
#endif
  
  std::string body = getBodyStr(req);
  if (!reader.parse(body, root)) {
    rc = -3;
    sendResponse(rc, "parse error", req, response);
    return;
  }
  
  std::shared_ptr<Chat> chat(new Chat());
  JsonUtil::getJsonString(root, "message", chat->message);
 
  if (chat->message.empty()) {
    rc = -4;
    sendResponse(rc, "message error", req, response);
    return;
  }
  ChatService &service = ChatService::getChatService(); 
  rc = service.saveChat(chat);
  evbuffer_add_printf(response, "%s", rc == 0 ? "success" : "fail");
  //evhtp_send_reply(req, EVHTP_RES_OK);
  evhttp_send_reply(req, 200, "ss", response);
}

void ChatControl::chatlatestChatCb(http_request *req, void *arg) {
  int rc = 0;
  Json::Value root;
  //evbuffer *response = req->buffer_out;
  evbuffer *response = evbuffer_new();
#if 0
  if (req->method != htp_method_POST) {
    rc = -1;
    sendResponse(rc, "method not support", req, response);
    return;
  }
#endif
  
  std::string body = getBodyStr(req);
  ChatService &service = ChatService::getChatService(); 
  rc = service.getLatestChat();
  root["maxId"] = rc;
  evbuffer_add_printf(response, "%s", root.toStyledString().c_str());
  //evhtp_send_reply(req, EVHTP_RES_OK);
  evhttp_send_reply(req, 200, "ss", response);
}

std::vector<HttpControl> ChatControl::getMapping() {
  std::vector<HttpControl> controlList = {
    {"/news/chat/detect", ChatControl::chatDetectCb},
    {"/news/chat/uploadChat", ChatControl::chatUploadChatCb},
    {"/news/chat/latestChat", ChatControl::chatlatestChatCb}
  };
  return controlList;
}

}

