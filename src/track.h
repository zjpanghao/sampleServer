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
#include "PersonSearch.h"
#include "detectService.h"
#include "detectServiceCvImpl.h"
#include "config/config.h"


namespace ktrack {
class Track : public KafkaConsumer {
 public:
   Track();
   ~Track();
   int init(const kunyan::Config &config);
   void ProcessMessage(const char *buf, int len) override;
   bool initClient();
   bool initPersonClient();
   cv::Mat getLatestImage();

 private:
   int errorConnectCount_{0};
   long errorTime_{0};
   int errorPersonConnectCount_{0};
   long errorPersonTime_{0};
   int index_{0};
   cv::Mat image_;
   cv::Rect2d move_;
   std::mutex lock_;
   FaceApi  faceApi_;
   std::shared_ptr<HelmetClient> client_;
   std::shared_ptr<PersonSearchClient> personClient_;
   std::unique_ptr<kface::DetectService> detectService_;
   cv::Mat right_[2];
   cv::Mat error_[2];
   volatile bool stop_{true};
};

} // namespace
#endif
