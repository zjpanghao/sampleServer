#include "faceControl.h"
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
#include "faceService.h"
#include "faceEntity.h"

namespace kface {
int FaceControl::faceDetectCb(const Json::Value &root,
    Json::Value &response) {
  struct timeval tv[2];
  int rc = 0;
  Json::Value faceResult;
  Json::Value items;
  Json::Reader reader;
 
  std::string data;
  
  JsonUtil::getJsonString(root, "image", data);
 
  if (data.empty()) {
   // response["errcode"] = -4;
   // return 0;
  }
  int faceNum = 1;
  JsonUtil::getJsonInt(root, "max_face_num", faceNum);
  std::string faceType;
  JsonUtil::getJsonString(root, "face_type", faceType);
  std::string decodeData;
  int decodeLen = 0;
#if 0
  std::vector<unsigned char> cdata;
  cdata.assign(&decodeData[0], &decodeData[0] + decodeLen);
#endif
  FaceService &service = FaceService::getFaceService(); 
  std::shared_ptr<News> news = service.getLatestNews();
  if (news) {
    response["errcode"] = 0;
    response["uid"] = news->id;
  } else {
    response["errcode"] = -2;
  }
  return 0;
}

int FaceControl::init(
    const kunyan::Config &config) {
  FaceService &service = FaceService::getFaceService();
  return service.init(config);
  
}

std::vector<HttpControl> FaceControl::getMapping() {
  std::vector<HttpControl> controlList = {
   {"/detect",FaceControl::faceDetectCb}
  };
  return controlList;
}

}

