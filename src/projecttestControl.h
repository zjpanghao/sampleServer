#ifndef INCLUDE_FACE_CONTROL_H
#define INCLUDE_FACE_CONTROL_H
#include <vector>
#include "evdrv/generalControl.h"
#include "json/json.h"
namespace kprojecttest {
class ProjecttestControl : public GeneralControl {
 public:
   /*support image type  base64*/
   static int projecttestDetectCb(const Json::Value &root,
       Json::Value &result);

   virtual std::vector<HttpControl> getMapping() override;
   virtual int init(const kunyan::Config &config);
};

}
#endif
