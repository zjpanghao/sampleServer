#ifndef INCLUDE_FACE_SERVICE_H
#define INCLUDE_FACE_SERVICE_H
#include <map>
#include <memory>

#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>

namespace kface {
class FaceService {
 public:
  static FaceService& getFaceService();
  FaceService();
};
}

#endif
