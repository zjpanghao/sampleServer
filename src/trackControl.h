#ifndef INCLUDE_TRACK_CONTROL_H
#define INCLUDE_TRACK_CONTROL_H
#include <memory>
#include <opencv2/opencv.hpp>
#include "pkafka/kafka_consumer.h"
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
class TrackControl : public KafkaConsumer {
 public:
   TrackControl(const kunyan::Config &config);
   ~TrackControl();
   int init();
   void ProcessMessage(const char *buf, int len) override;
   std::shared_ptr<Track> getTrackById(int caseId);
    
 private:
   std::map<int, std::shared_ptr<Track>> trackMp_;
   const kunyan::Config &config_;
   std::mutex lock_;
   ApiBuffer<DetectServiceCvImpl> detectBuffers_;
   ApiBuffer<FaceApi> faceBuffers_;
   ApiBuffer<HelmetClientDelegation> helmetClients_;
   std::shared_ptr<ExecutorService> executorService_{nullptr};
   ConfigParam  configParam_;
   static constexpr int DETECT_BUFFER_NUM{3};
   static constexpr int FACE_BUFFER_NUM{3};
};

} // namespace
#endif
