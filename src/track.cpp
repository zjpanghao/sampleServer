#include "track.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include "pbase64/base64.h"
#include <json/json.h>
#include <glog/logging.h>
#include "faceEntity.h"
#include "detectService.h"
#include "detectServiceCvImpl.h"

using  apache::thrift::protocol::TProtocol;
using  apache::thrift::protocol::TBinaryProtocol;
using  apache::thrift::transport::TTransport;
using  apache::thrift::transport::TSocket;
using  apache::thrift::transport::TTransportException;
using  apache::thrift::transport::TBufferedTransport;

#define PERSON_CONF  0.5
#define BORDER_WIDTH 150
namespace ktrack {
Track::Track() : detectService_(new kface::DetectServiceCvImpl()) {

}

Track::~Track() {
  if (client_ != nullptr) {
    client_->getInputProtocol()->getTransport()->close();
  }
  if (personClient_ != nullptr) {
    personClient_->getInputProtocol()->getTransport()->close();
  }
  stop();
  LOG(INFO) << "track task exit";
}

bool Track::initClient() {
  std::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
  std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  client_ = std::make_shared<HelmetClient>(protocol);
  try {
    transport->open();
  } catch (TTransportException &e) {
    LOG(ERROR) << e.what();
    return false;
  }
  return true;
}

bool Track::initPersonClient() {
  LOG(INFO) << "person client init";
  std::shared_ptr<TTransport> socket(new TSocket("localhost", 9091));
  std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  personClient_ = std::make_shared<PersonSearchClient>(protocol);
  try {
    transport->open();
  } catch (TTransportException &e) {
    LOG(ERROR) << e.what();
    return false;
  }
  return true;
}

int Track::init(const kunyan::Config &config) {
  detectService_->init(config);
  int rc = Init("192.168.1.111:9092", "face_11", "group_face");
  if (rc == 0) {
    rc |= StartAll();
  }
  if (rc != 0) {
    return -1;
  }
  index_ = 0;
  right_[0] = cv::imread("right.jpg");
  error_[0] = cv::imread("error.jpg");
  right_[1] = cv::imread("right.jpg", 0);
  error_[1] = cv::imread("error.jpg", 0);
  if (!initClient()) {
    LOG(ERROR) << "rpc init failed";
    return -1;
  }
#if 1
  if (!initPersonClient()) {
    LOG(ERROR) << "rpc init failed";
    return -1;
  }
#endif
  return 0;
}

cv::Mat Track::getLatestImage() {
  std::lock_guard<std::mutex> lck(lock_);
  return image_;
}

static bool errorRc(int rc) {
  return rc == 1 ? false : true;
}

static int getPersonSize(std::vector<kface::ObjectDetectResult> &persons) {
  int count = 0;
  for (auto &face : persons) {
    if (face.category != "person" || face.score < PERSON_CONF) {
      continue;
    }
    count++;
  }
  return count;

}

void Track::ProcessMessage(const char *buf, int len) {
  std::string recv(buf, len);
  Json::Value root;
  Json::Reader reader;
  bool f = reader.parse(recv, root);
  if (!f) {
    return;
  }
  std::string image = root["image"].asString();
  std::vector<unsigned char> data;
  Base64::getBase64().decode(image, data);
  cv::Mat mo = cv::imdecode(data, cv::ImreadModes::IMREAD_COLOR );
  cv::Mat m(mo, cv::Rect(mo.cols / 3, 0, mo.cols * 2 / 3 - 1 , mo.rows));
  m = m.clone();
#if 0
  PersonSearchResult personSearchResult;
  try {
      bool status = true;
      if (errorPersonConnectCount_ >= 1 && time(NULL) > errorPersonTime_ + 3) {
        status = initPersonClient();
        errorPersonConnectCount_ = 0;
        errorPersonTime_ = time(NULL);
      }
      if (status) {
        personClient_->search(personSearchResult, image);
        errorPersonConnectCount_ = 0;
      }
  } catch(std::exception &e) {
    errorPersonConnectCount_++;
    errorPersonTime_ = time(NULL);
  }
  if (personSearchResult.errorCode != 0) {
    LOG(INFO) << "errorCode:" << personSearchResult.errorCode;
  }
  if (personSearchResult.result.size() > 0) {
    LOG(INFO) << "person count:" << personSearchResult.result.size();
  }
  if (personSearchResult.errorCode == 0 && personSearchResult.result.size() == 0) {
    std::lock_guard<std::mutex> lck(lock_);
    image_ = m;
    return;
  }
#endif

  std::vector<kface::ObjectDetectResult> faces;
  detectService_->getDetectResult(m, faces);
  int count = getPersonSize(faces);
//  int count = personSearchResult.result.size();
  if (count > 0) {
    cv::Mat showImage;
    if (count > 1) {
      showImage = m.clone();
    } else if (count == 1){
      showImage = m;
    }
    for (auto &face : faces) {
    //for (auto &face : personSearchResult.result) {
     LOG(INFO) << "(" << face.x <<"," << face.y << "," << face.width << "," << face.height <<"," << face.score;
     if (face.category != "person" || face.score < PERSON_CONF) {
        continue;
     }
      //cv::Rect box = face.rect();
      if (face.x < BORDER_WIDTH) {
        continue;
      }

      if (face.x > m.cols - BORDER_WIDTH) {
        continue;
      }

      if (face.x >= m.cols || face.y >= m.rows) {
        continue;
      }
      if (face.y < 0) {
        face.y = 0;
      }
      if (face.width  + face.x > m.cols) {
        face.width = m.cols - face.x;
      }
      if (face.height > m.rows - face.y) {
        face.height = m.rows - face.y;
      }
      if (face.width <= 0 || face.height <= 0) {
        continue;
      }
      cv::Rect box(face.x, face.y, face.width, face.height/6);
      cv::Mat faceImage(m, box);
      std::vector<FaceLocation> faceList;
      faceApi_.getLocations(faceImage, faceList);
      auto boxDetect = box;
      if (faceList.size() > 0) {
        auto &faceCut = faceList[0];
        if (faceCut.confidence() > 0.5) {
          box = faceCut.rect();
        }
        box.x -= box.width / 2;
        if (box.x < 0) {
          box.x = 0;
        }
        box.y -= box.height / 2;
        if (box.y < 0) {
          box.y = 0;
        }
        box.height +=  box.height / 2;
        if (box.y + box.height >= faceImage.rows) {
          box.height = faceImage.rows - box.y;
        }

        box.width  = box.width * 2;
        if (box.width + box.x >= faceImage.cols) {
          box.width = faceImage.cols - box.x;
        }
        faceImage = cv::Mat(faceImage, box);
        box.x  += boxDetect.x;
        box.y  += boxDetect.y;
      }
#if 1
        std::stringstream file1;
        file1 << "landmark/" << "landmark_" << index_ << ".jpg";
        index_ = (index_ + 1) % 1000;
        if (faceImage.cols < faceImage.rows * 1.7) {
          cv::imwrite(file1.str(), faceImage);
        }
#endif
      std::vector<unsigned char> faceImageData;
      cv::imencode(".jpg", faceImage, faceImageData);
      std::string imageBase64;
      Base64::getBase64().encode(faceImageData, imageBase64);
      int rc = -1;
      bool status = true;
      try {
        if (errorConnectCount_ >= 1 && time(NULL) > errorTime_ + 3) {
          status = initClient();
          errorConnectCount_ = 0;
          errorTime_ = time(NULL);
        }
        if (status) {
          rc = client_->checkHelmet(imageBase64);
        }
        LOG(INFO) << "recv " << rc;
        errorConnectCount_ = 0;
      } catch (::apache::thrift::transport::TTransportException &e) {
        LOG(ERROR) << "transport exception:" << e.what();
        errorConnectCount_++;
        if (errorConnectCount_ == 1) {
          errorTime_ = time(NULL);
        }
        rc = -1;
      } catch (::apache::thrift::TApplicationException &e) {
        LOG(ERROR) << "check helemt exception" << e.what();
        rc = -1;
      } catch (std::exception &e) {
        LOG(ERROR) << "check helemt std::exception" << e.what();
        rc = -1;
      } 
      static cv::Scalar  RED(0, 0, 255);
      static cv::Scalar  GREEN(0, 255, 0);
      static cv::Scalar BLUE(255, 0, 0);
      cv::Scalar  &scalar = BLUE;
      
      if (rc > 0 ) {
        scalar = (errorRc(rc)) ? RED :GREEN;
        cv::Mat &alert = (errorRc(rc)) ? error_[0] : right_[0];
        cv::Mat &alertMask = (errorRc(rc)) ? error_[1] : right_[1];
        cv::Rect alertBox(box.x, box.y - alert.rows < 0 ? 0 : box.y - alert.rows, 
            alert.cols,  alert.rows);
        cv::Mat alertImage(showImage, alertBox);
        alert.copyTo(alertImage, alertMask);
      }
      if (rc != -2) {
        cv::rectangle(showImage, box, scalar, 2, 1);
      }
    }
    
    std::stringstream file;
    file << "cv/" << index_ << ".jpg";
    index_ = (index_ + 1) % 1000;
    cv::imwrite(file.str(), showImage);
    m = showImage;
  } 
  std::lock_guard<std::mutex> lck(lock_);
  image_ = m;
}

} //namespace ktrack
