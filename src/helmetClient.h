#ifndef INCLUDE_HELMET_CLIENT_H
#define INCLUDE_HELMET_CLIENT_H
#include "Helmet.h"
#include <memory>
#include "config/config.h"
class HelmetClientDelegation {
 public:
   bool initClient(); 
   void init(int number, const kunyan::Config &config);
   void init(const kunyan::Config &config,
            const std::string &tag);
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
   int recvTimeout_{15};
   int port_;
};


#endif
