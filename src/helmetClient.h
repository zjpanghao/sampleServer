#ifndef INCLUDE_HELMET_CLIENT_H
#define INCLUDE_HELMET_CLIENT_H
#include "Helmet.h"
#include <memory>
#include "config/config.h"
class HelmetClientDelegation {
 public:
   bool initClient(); 
   void init(const kunyan::Config &config);
   int errorConnectCount_{0};
   long errorTime_{0};
   std::shared_ptr<HelmetClient>  client();

   bool needReconnect() {
     return errorConnectCount_ >= 1 && time(NULL) > errorTime_ + 3;
   }

   bool status_{false};

 private:
   std::shared_ptr<HelmetClient> client_;
   std::string server_;
   int port_;
};


#endif
