#include <iostream>
#include <string>
#include <stdio.h>
#include <regex>
#include <sstream>
#include <fstream>
#include <iterator>
#include <thread>
#include "json/json.h"
#include "config/config.h"
#include <chrono>
#include "glog/logging.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include "faceService.h"
#include "detectService.h"
#include "db/dbpool.h"
#include <memory>
#include "faceControl.h"

using kface::FaceService;
using ktrack::DetectService;
extern void ev_server_start_multhread(const kunyan::Config &config, const std::vector<std::shared_ptr<kface::GeneralControl>> &controls);

static void initGlog(const std::string &name) {
  DIR *dir = opendir("log");
  if (!dir) {
    mkdir("log", S_IRWXU);
  } else {
    closedir(dir);
  }
  google::InitGoogleLogging(name.c_str());
  google::SetLogDestination(google::INFO,std::string("log/"+ name + "info").c_str());
  google::SetLogDestination(google::WARNING,std::string("log/"+ name + "warn").c_str());
  google::SetLogDestination(google::GLOG_ERROR,std::string("log/"+ name + "error").c_str());
  FLAGS_logbufsecs = 0;
}

int main(int argc, char *argv[]) {
  std::string name(argv[0]);
  daemon(1, 0);
  initGlog(name);
  kunyan::Config config("config.ini");
  FaceService &service = FaceService::getFaceService();
  service.init(config);
  static std::vector<std::shared_ptr<kface::GeneralControl>> controls{std::make_shared<kface::FaceControl>()};
  ev_server_start_multhread(config, controls); 
  while (1) {
    ::sleep(10000);
  }
  return 0;
}
