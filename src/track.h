#ifndef INCLUDE_TRACK_H
#define INCLUDE_TRACK_H
#include <memory>
#include <atomic>
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
#include "threadpool/thread_pool.h"
#include "helmetClient.h"
#include "videoInfo.h"
#include <opencv2/video/background_segm.hpp>
struct HelmetMatData;
class HelmetControlInfo;
class ObjectMoveDetect;
namespace ktrack {
class Track  {
 public:
   Track(int trackId, 
           ApiBuffer<DetectServiceCvImpl> &detectApiBuffers,
           ApiBuffer<FaceApi> &faceApiBuffers, 
           ApiBuffer<HelmetClientDelegation> &helmetClients,
           const ConfigParam &configParam);
   ~Track();
   Track(const Track &) = delete;
   Track & operator = (const Track &) = delete;
   int init();
   cv::Mat getLatestImage();
   void detectObjects(const cv::Mat &m,
       std::vector<ObjectDetectResult> &persons);

   int detectPersonInfo(const cv::Mat &m,
      std::vector<DetectInfo> &results);
   int detectFaceInfo(const cv::Mat &m,
      std::vector<DetectInfo> &results);
   int preDetect(const cv::Mat &m,
      const cv::Mat &mask,
      std::vector<DetectInfo> &results);
   int detect(const cv::Mat &m,
      std::vector<DetectInfo> &results);
   double getWhiteRate(const cv::Mat &m);
   void doDrawWork(cv::Mat &mo, 
       const cv::Mat &bgmask,
       const std::vector<DetectInfo> &results);

 private:
   void setRect(const cv::Rect &rect, ObjectDetectResult &result) {
    result.x = rect.x;
    result.y = rect.y;
    result.width = rect.width;
    result.height = rect.height;
   }

  cv::Rect  getRect(const ObjectDetectResult &result) {
    return cv::Rect(result.x, 
                    result.y,
                    result.width,
                    result.height);
  }
  void getFaceBox(const cv::Rect &originRect, int maxWidth, cv::Rect &faceRect);

void fineHelmetBoxHorizontalByBG(
    int horizontalThresh,
    const cv::Mat &bgmask,
    std::vector<DetectInfo> &results);
  void fineHelmetBoxByBackground(
      int widthThresh,
      const cv::Mat &bgmask,
      std::vector<DetectInfo> &results); 
  void fineHelmetBoxByFace(
      const cv::Mat &m,
      std::vector<DetectInfo> &results);

   int index_{0};
   std::shared_ptr<VideoInfo> videoInfo_{nullptr};//{std::make_shared<VideoInfo>()};
   std::shared_ptr<HelmetClient> client_;
   int trackId_{-1};
   ApiBuffer<DetectServiceCvImpl> &detectBuffers_;
   ApiBuffer<FaceApi> &faceBuffers_;
   ApiBuffer<HelmetClientDelegation> &clients_;
   std::shared_ptr<HelmetControlInfo> helmetControlInfo_;
   ConfigParam configParam_;
   std::shared_ptr<ObjectMoveDetect> moveDetect_;
};

} // namespace
#endif
