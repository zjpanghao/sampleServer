#include "videoInfo.h"
    void     VideoInfo::updateImage(const cv::Mat &m) {
      std::lock_guard<std::mutex> guard(lock_);
      if (m.cols == 0) {
        imageBase64_ =  "";
        return;
      }
      std::vector<unsigned char> data;
      cv::imencode(".jpg", m, data);
      imageBase64_ = "";
      Base64::getBase64().encode(data, 
          imageBase64_);
    }

    void VideoInfo::getImage(std::string &image) {
      std::lock_guard<std::mutex> guard(lock_);
      static std::string imageHead = "data:image/jpeg;base64,";
      image =  imageHead + imageBase64_;
    }
