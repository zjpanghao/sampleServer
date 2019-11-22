#ifndef INCLUDE_VIDEOINFO_H
#define INCLUDE_VIDEOINFO_H
#include <opencv2/opencv.hpp>
#include <memory>
#include <mutex>

class VideoInfo {
  public:
    VideoInfo() = default;
    void updateImage(const cv::Mat &m) {
      std::lock_guard<std::mutex> guard(lock_);
      image_ = m;
    }

    cv::Mat getImage() {
      std::lock_guard<std::mutex> guard(lock_);
      return image_;
    }

    VideoInfo(const VideoInfo &) = delete;
    VideoInfo &operator=(const VideoInfo &) = delete;

  private:
    cv::Mat image_;
    std::mutex lock_;
};

#endif
