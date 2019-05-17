#ifndef INCLUDE_FACE_CONTROL_H
#define INCLUDE_FACE_CONTROL_H
#include <vector>
#include "httpUtil.h"
#include "generalControl.h"
struct evhttp_request;
namespace knews {
class FaceControl : public GeneralControl {
 public:
   /* can accept image_type FACE_TOKEN & BASE64*/
   static void faceIdentifyCb(http_request *req, void *arg);
   /*support image type  base64*/
   static void faceDetectCb(http_request *req, void *arg);
   static void faceUploadNewsCb(http_request *req, void *arg);

   virtual std::vector<HttpControl> getMapping() override;
};

}

#endif
