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

namespace kface {
void FaceControl::faceDetectCb(struct evhtp_request *req, void *arg) {
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
  std::string data;
  
  getJsonString(root, "image", data);
 
  if (data.empty()) {
    rc = -4;
    sendResponse(rc, "image error", req, response);
    return;
  }
  int faceNum = 1;
  getJsonString(root, "max_face_num", faceNum);
  std::string faceType;
  getJsonString(root, "face_type", faceType);
  std::string decodeData;
  int decodeLen = 0;
  std::vector<unsigned char> cdata;
  cdata.assign(&decodeData[0], &decodeData[0] + decodeLen);
  FaceService &service = FaceService::getFaceService(); 
  std::shared_ptr<News> news = service.getLatestNews();
  std::string result = news->url;
  if (news != nullptr) {
    result = news->abstract;
  }
  evbuffer_add_printf(response, "%s", result.c_str());
  //evhttp_send_reply(req, 200, "OK", response);
  evhtp_send_reply(req, EVHTP_RES_OK);
}

std::vector<HttpControl> FaceControl::getMapping() {
  std::vector<HttpControl> controlList = {
    {"/detect", FaceControl::faceDetectCb}
  };
  return controlList;
}

}

