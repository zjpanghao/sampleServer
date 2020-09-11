#include "projecttestControl.h"
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
#include "projecttestService.h"
#include "projecttestEntity.h"

namespace kprojecttest {
int ProjecttestControl::projecttestDetectCb(const Json::Value &root,
    Json::Value &response) {
  struct timeval tv[2];
  int rc = 0;
  Json::Value projecttestResult;
  Json::Value items;
  Json::Reader reader;
 
  std::string data;
  
  JsonUtil::getJsonString(root, "image", data);
 
  if (data.empty()) {
   // response["errcode"] = -4;
   // return 0;
  }
  int projecttestNum = 1;
  JsonUtil::getJsonInt(root, "max_projecttest_num", projecttestNum);
  std::string projecttestType;
  JsonUtil::getJsonString(root, "projecttest_type", projecttestType);
  std::string decodeData;
  int decodeLen = 0;
#if 0
  std::vector<unsigned char> cdata;
  cdata.assign(&decodeData[0], &decodeData[0] + decodeLen);
#endif
  ProjecttestService &service = ProjecttestService::getProjecttestService(); 
  std::shared_ptr<News> news = service.getLatestNews();
  if (news) {
    response["errcode"] = 0;
    response["uid"] = news->id;
  } else {
    response["errcode"] = -2;
  }
  return 0;
}

int ProjecttestControl::init(
    const kunyan::Config &config) {
  ProjecttestService &service = ProjecttestService::getProjecttestService();
  return service.init(config);
  
}

std::vector<HttpControl> ProjecttestControl::getMapping() {
  std::vector<HttpControl> controlList = {
   {"/detect",ProjecttestControl::projecttestDetectCb}
  };
  return controlList;
}

}

