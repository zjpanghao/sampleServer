#include "emsControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/util.h>

#include "json/json.h"
#include "event2/http.h"


#include <thread>
#include <vector>
#include <glog/logging.h>
#include <iterator>
#include <regex>
#include "jsonUtil.h"
#include "emsService.h"
#include "emsEntity.h"

namespace kems {
int EmsControl::emsMessageCb(const pson::Json::Value &root,
    pson::Json::Value &response) {
  struct timeval tv[2];
  int rc = 0;
  pson::Json::Value emsResult;
  pson::Json::Value items;
  pson::Json::Reader reader;
 
  std::string body ="";
  std::string to;
  std::string subject = "";
  
  JsonUtil::getJsonString(root, "body", body);
  JsonUtil::getJsonString(root, "to", to);
  JsonUtil::getJsonString(root, "subject", subject);
 
  if (to.empty()) {
    response["errcode"] = -1;
    return 0;
  }

  EmsService &service = EmsService::getEmsService(); 
  rc = service.sendEms(
      subject,body, to
      );
  response["errcode"] = rc;
  return 0;
}

int EmsControl::init(
    const kunyan::Config &config) {
  EmsService &service = EmsService::getEmsService();
  return service.init(config);
  
}

std::vector<HttpControl> EmsControl::getMapping() {
  std::vector<HttpControl> controlList = {
   {"/ems/message",EmsControl::emsMessageCb}
  };
  return controlList;
}

}

