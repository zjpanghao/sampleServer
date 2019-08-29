#include "helmetTask.h"
#include <glog/logging.h>
#include "pbase64/base64.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "apipool/apiPool.h"


HelmetControlInfo::HelmetControlInfo(std::shared_ptr<HelmetMatData> matData, 
                    ApiBuffer<HelmetClientDelegation> &clients,
                    std::shared_ptr<VideoInfo> videoCb) 
  : clients_(clients), videoCb_(videoCb), matData_(matData){

}

void HelmetControlInfo::doDrawWork() {
  for (auto &checkInfo : checkInfoList_) {    
    auto &box = checkInfo->rect;
    if (checkInfo->result.errorCode == 0) {
      int rc = checkInfo->result.index;
      cv::Scalar &scalar = Cvcolor::color().colors[rc];
      cv::Mat &alert = (rc != 1) ? matData_->error[0] : matData_->right[0];
      cv::Mat &alertMask = (rc != 1) ? matData_->error[1] : matData_->right[1];
      cv::Rect alertBox(box.x, box.y - alert.rows < 0 ? 0 : box.y - alert.rows, 
          alert.cols,  alert.rows);
      cv::Mat alertImage(m_, alertBox);
      alert.copyTo(alertImage, alertMask);
      cv::rectangle(m_, box, scalar, 2, 1);
    }
  }
  videoCb_->updateImage(m_);
  std::lock_guard<std::mutex> guard(lock);
  done = true;
  cond.notify_one();
}

void HelmetControlInfo::run(int inx) {
  if (checkInfoList_.empty()) {
    return;
  }
  auto checkInfo = checkInfoList_[inx];
  int ret = checkHelmet(checkInfo->personImage, checkInfo->result);
  if (ret < 0) {
    checkInfo->result.errorCode = -1;
  }
  if (1 == number_.fetch_add(-1)) {
    doDrawWork();
  }
}

int HelmetControlInfo::checkHelmet(const cv::Mat &detectImage, HelmetCheckResult &result) {
  std::vector<unsigned char> faceImageData;
  cv::imencode(".jpg", detectImage, faceImageData);
  std::string imageBase64;
  Base64::getBase64().encode(faceImageData, imageBase64);
  ApiWrapper<HelmetClientDelegation> wrapper(clients_);
  auto client = wrapper.getApi();
  auto tClient = client->client();
  if (tClient == nullptr) {
    LOG(ERROR) << "get client error";
    return -1;
  }
  int rc = -1;
  try {
      tClient->checkHelmet(result, imageBase64);
      rc = 0;
  } catch (::apache::thrift::transport::TTransportException &e) {
      LOG(ERROR) << "transport exception:" << e.what();
  } catch (::apache::thrift::TApplicationException &e) {
      LOG(ERROR) << "check helemt exception" << e.what();
      rc = -2;
  } catch (std::exception &e) {
      LOG(ERROR) << "check helemt std::exception" << e.what();
      rc = -3;
  } 
  return rc;
}



void HelmetControlInfo::ErrorMsg(int code, const std::string &msg) {
  if (code == 1) {
    LOG(INFO) << "queue full notify one";
    std::lock_guard<std::mutex> guard(lock);
    done = true;
    cond.notify_one();
  }
}

HelmetTask::HelmetTask(std::shared_ptr<HelmetArg> arg):arg_(arg) {
}

void HelmetTask::ErrorMsg(int code, const std::string &msg) {
  arg_->info->ErrorMsg(code, msg);
}

void HelmetTask::Run() {
  int inx = arg_->personIndex;
  arg_->info->run(inx);
}

