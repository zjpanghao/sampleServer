#ifndef INCLUDE_TRACK_CONTROL_H
#define INCLUDE_TRACK_CONTROL_H
#include <memory>
#include <opencv2/opencv.hpp>
#include <mutex>
#include "faceApi.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "Helmet.h"
#include "detectService.h"
#include "detectServiceCvImpl.h"
#include "config/config.h"
#include "apipool/apiPool.h"
#include "track.h"
#include "threadpool/thread_pool.h"
#include "helmetClient.h"
namespace ktrack {
class TrackControl {
 public:
   TrackControl(const kunyan::Config &config);
   ~TrackControl();
   int init();
   void startTrack(int n);
   std::shared_ptr<Track> getTrackById(int caseId);
    
 private:
   template<class T>
     void setConfig(
         const std::string &segment,
         const std::string &key,
         T   &param,
         T   defaultValue);
   std::map<int, std::shared_ptr<Track>> trackMp_;
   const kunyan::Config &config_;
   ApiBuffer<DetectServiceCvImpl> detectBuffers_;
   ApiBuffer<FaceApi> faceBuffers_;
   static constexpr int CLIENTS_NUM = 5;
   ApiBuffer<HelmetClientDelegation> helmetClients_[CLIENTS_NUM];
   std::shared_ptr<ExecutorService> executorService_{nullptr};
   ConfigParam  configParam_;
   static constexpr int DETECT_BUFFER_NUM{3};
   static constexpr int FACE_BUFFER_NUM{3};
};

} // namespace
#endif
