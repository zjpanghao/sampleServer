#ifndef INCLUDE_VIDEOINFO_H
#define INCLUDE_VIDEOINFO_H
#include <opencv2/opencv.hpp>
#include <memory>
#include <mutex>
#include "pbase64/base64.h"

class VideoInfo {
  public:
    VideoInfo() = default;
    void updateImage(const cv::Mat &m); 

    void getImage(std::string &image);

    VideoInfo(const VideoInfo &) = delete;
    VideoInfo &operator=(const VideoInfo &) = delete;

  private:
    cv::Mat image_;
    std::string imageBase64_;
    std::mutex lock_;
};

#endif
