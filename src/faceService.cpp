#include "faceService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "util.h"
#include "faceConst.h"

namespace kface {
FaceService& FaceService::getFaceService() {
  static FaceService faceService;
  return faceService;
}
FaceService::FaceService() {
}
}
