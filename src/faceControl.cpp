#include "faceControl.h"
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

#include "faceService.h"
#include "faceEntity.h"
#include <pbase64/base64.h>

namespace kface {
void FaceControl::faceDetectCb(struct evhtp_request *req, void *arg) {
  struct timeval tv[2];
  int rc = 0;
  Json::Value root;
  Json::Reader reader;
 
  evbuffer *response = req->buffer_out;
  if (req->method != htp_method_POST) {
    rc = -1;
    sendResponse(rc, "method not support", req, response);
    return;
  }
  
  std::string body = getBodyStr(req);
  if (!reader.parse(body, root)) {
    rc = -3;
    sendResponse(rc, "parse error", req, response);
    return;
  }
  
  int newsId = 0;
  int size = 2;
  JsonUtil::getJsonInt(root, "newsId", newsId);
  JsonUtil::getJsonInt(root, "size", size);
 
  if (newsId == 0) {
    rc = -4;
    sendResponse(rc, "newsId error", req, response);
    return;
  }
  LOG(INFO) << newsId << "," << size;
  //getJsonInt(root, "max_face_num", faceNum);
  //std::string decodeData;
  //int decodeLen = 0;
  //std::vector<unsigned char> cdata;
  //cdata.assign(&decodeData[0], &decodeData[0] + decodeLen);
  gettimeofday(&tv[0], NULL);
  FaceService &service = FaceService::getFaceService(); 
  std::list<std::shared_ptr<News>> newsList = service.getLatestNewsMore(newsId, size);
  gettimeofday(&tv[1], NULL);
  LOG(INFO) << tv[0].tv_sec << ":" << tv[0].tv_usec;
  LOG(INFO) << tv[1].tv_sec << ":" << tv[1].tv_usec;
  LOG(INFO) << "get news size:" << newsList.size();
  Json::Value items;
  for (auto &news : newsList) { 
    Json::Value item;
    item["abstract"] = news->abstract;
    item["newsId"] = news->id;
    item["image"] =  news->image.imageBase64;
    item["head"] =  news->image.head;
    items.append(item);
  }
  Json::Value res;
  res["items"] = items;
  evbuffer_add_printf(response, "%s", res.toStyledString().c_str());
  evhtp_send_reply(req, EVHTP_RES_OK);
}

void FaceControl::faceUploadNewsCb(struct evhtp_request *req, void *arg) {
  struct timeval tv[2];
  int rc = 0;
  Json::Value root;
  Json::Value faceResult;
  Json::Value items;
  Json::Reader reader;
 
  evbuffer *response = req->buffer_out;
  if (req->method != htp_method_POST) {
    rc = -1;
    sendResponse(rc, "method not support", req, response);
    return;
  }
  
  std::string body = getBodyStr(req);
  if (!reader.parse(body, root)) {
    rc = -3;
    sendResponse(rc, "parse error", req, response);
    return;
  }
  
  std::shared_ptr<News> news(new News());
  JsonUtil::getJsonString(root, "image", news->image.imageBase64);
  JsonUtil::getJsonString(root, "head", news->image.head);
 
  if (news->image.imageBase64.empty()) {
    rc = -4;
    sendResponse(rc, "image error", req, response);
    return;
  }
  std::string tmp;
  int faceNum = 1;
  JsonUtil::getJsonString(root, "max_face_num", tmp);
  if (!tmp.empty()) {
    Util::convert(tmp, faceNum);
  }
  //getJsonInt(root, "max_face_num", faceNum);
  JsonUtil::getJsonString(root, "abstract", news->abstract);
  FaceService &service = FaceService::getFaceService(); 
  rc = service.saveNews(news);
  evbuffer_add_printf(response, "%s", rc == 0 ? "success" : "fail");
  evhtp_send_reply(req, EVHTP_RES_OK);
}


std::vector<HttpControl> FaceControl::getMapping() {
  std::vector<HttpControl> controlList = {
    {"/news/detect", FaceControl::faceDetectCb},
    {"/news/uploadNews", FaceControl::faceUploadNewsCb}
  };
  return controlList;
}

}

