
#ifndef INCLUDE_CHAT_CONTROL_H
#define INCLUDE_CHAT_CONTROL_H
#include <vector>
#include "httpUtil.h"
#include "generalControl.h"
struct evhttp_request;
namespace knews {
class ChatControl : public GeneralControl {
 public:
   /* can accept image_type CHAT_TOKEN & BASE64*/
   static void chatIdentifyCb(http_request *req, void *arg);
   /*support image type  base64*/
   static void chatDetectCb(http_request *req, void *arg);
   static void chatUploadChatCb(http_request *req, void *arg);
   static void chatlatestChatCb(http_request *req, void *arg);

   virtual std::vector<HttpControl> getMapping() override;
};

}

#endif
