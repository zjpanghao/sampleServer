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
void FaceControl::trackImageCb(struct evhttp_request *req, void *arg) {
  int rc = 0;
  Json::Value root;
  Json::Reader reader;
   evbuffer *response = evbuffer_new();
  if (evhttp_request_get_command(req) != EVHTTP_REQ_POST) {
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
  int caseId = -1;
  getJsonInt(root, "caseId", caseId);
  if (caseId == -1) {
    rc = -4;
    sendResponse(rc, "caseId error", req, response);
    return;
  }
  FaceService &service = FaceService::getFaceService(); 
  std::string image = service.getLatestImage(caseId);
  evbuffer_add_printf(response, "%s", image.c_str());
  evhttp_send_reply(req, 200, "OK", response);
}

void FaceControl::trackStopCb(struct evhttp_request *req, void *arg) {
  FaceService &service = FaceService::getFaceService(); 
  evbuffer *response = evbuffer_new();
  service.trackStop();
  evbuffer_add_printf(response, "%s", "ok");
  evhttp_send_reply(req, 200, "OK", response);
}

void FaceControl::trackCb(struct evhttp_request *req, void *arg) {
  struct timeval tv[2];
  int rc = 0;
  Json::Value root;
  Json::Value faceResult;
  Json::Value items;
  Json::Reader reader;
 
  evbuffer *response = evbuffer_new();
#if 0
  if (evhttp_request_get_command(req) != EVHTTP_REQ_POST) {
    rc = -1;
    sendResponse(rc, "method not support", req, response);
    return;
  }
#endif
  

#if 0
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
#endif
  FaceService &service = FaceService::getFaceService(); 
  int id =0;
  id = service.trackStart();
  std::stringstream ss;
  ss << id;
  std::string result = ss.str();
  evbuffer_add_printf(response, "%s", result.c_str());
  evhttp_send_reply(req, 200, "OK", response);
}

std::vector<HttpControl> FaceControl::getMapping() {
  std::vector<HttpControl> controlList = {
    {"/face", FaceControl::trackCb},
    {"/face/stop", FaceControl::trackStopCb},
    {"/face/image", FaceControl::trackImageCb}
  };
  return controlList;
}

}

