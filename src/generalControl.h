#ifndef INCLUDE_GENERAL_CONTROL_H
#define INCLUDE_GENERAL_CONTROL_H
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "httpUtil.h"
namespace kface {
class GeneralControl {
 public:
  virtual std::vector<HttpControl> getMapping() = 0;
  virtual ~GeneralControl() = default;  
};
}
#endif
