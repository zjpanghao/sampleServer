#ifndef INCLUDE_TRACK_H
#define INCLUDE_TRACK_H
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
#include "threadpool/thread_pool.h"
#include "helmetClient.h"
#include "videoInfo.h"

namespace ktrack {
class Track : public KafkaConsumer {
 public:
   Track(ApiBuffer<DetectServiceCvImpl> &detectApiBuffers,
           ApiBuffer<FaceApi> &faceApiBuffers, ApiBuffer<HelmetClientDelegation> &helmetClients);
   ~Track();
   Track(const Track &) = delete;
   Track & operator = (const Track &) = delete;
   int init(std::shared_ptr<ExecutorService> executorService,
            const std::string &kafkaServer,
              const std::string &topic,
              const std::string &group);
   void ProcessMessage(const char *buf, int len) override;
   bool initClient();
   bool initPersonClient();
   cv::Mat getLatestImage();

 private:
   void filterPersons(std::vector<ObjectDetectResult> &persons, 
                            int maxWidth, int maxHeight);
   bool rectValid(const ObjectDetectResult &object, 
                      int maxWidth, int maxHeight);
   bool getHelmetBox(std::vector<FaceLocation> &faces, int maxWidth, int maxHeight, cv::Rect &rect);
   int checkHelmet(const cv::Mat &detectImage, HelmetCheckResult &result);
   int errorConnectCount_{0};
   long errorTime_{0};
   int errorPersonConnectCount_{0};
   long errorPersonTime_{0};
   int index_{0};
   std::shared_ptr<VideoInfo> videoInfo_{nullptr};//{std::make_shared<VideoInfo>()};
   cv::Rect2d move_;
   std::mutex lock_;
   //FaceApi  faceApi_;
   std::shared_ptr<HelmetClient> client_;
  // std::unique_ptr<kface::DetectService> detectService_;
   cv::Mat right_[2];
   cv::Mat error_[2];
   ApiBuffer<DetectServiceCvImpl> &detectBuffers_;
   ApiBuffer<FaceApi> &faceBuffers_;
   ApiBuffer<HelmetClientDelegation> &clients_;
   std::shared_ptr<ExecutorService> executorService_;
   volatile bool stop_{true};
};

} // namespace
#endif