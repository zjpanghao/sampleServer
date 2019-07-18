#include "util.h"
void getJsonInt(const Json::Value &value, const std::string &key, int &t) {
  if (value.isNull() || !value[key].isInt()) {
    return;
  }
  t = value[key].asInt();
}
