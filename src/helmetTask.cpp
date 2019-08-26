#include "helmetTask.h"
#include <glog/logging.h>
#include "pbase64/base64.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "apipool/apiPool.h"

HelmetControlInfo::HelmetControlInfo(const CheckInfoList &list) 
  : checkInfoList_(list), number_(list.size()){

}

HelmetTask::HelmetTask(std::shared_ptr<HelmetArg> arg):arg_(arg) {

}

void HelmetTask::doDrawWork() {
  static cv::Scalar  RED(0, 0, 255);
  static cv::Scalar  GREEN(0, 255, 0);
  static cv::Scalar BLUE(255, 0, 0);
  static cv::Scalar WHITE(255, 255, 255);
  cv::Scalar *colors[] = {&RED, &GREEN, &BLUE, &WHITE};
  cv::Scalar  &scalar = BLUE;
  for (auto &checkInfo : arg_->info->checkInfoList_) {    
    auto &box = checkInfo->rect;
    if (checkInfo->result.errorCode == 0) {
      int rc = checkInfo->result.index;
      scalar = *colors[rc];
#if 0
      cv::Mat &alert = (rc != 1) ? arg_->info->error[0] : arg_->info->right[0];
      cv::Mat &alertMask = (rc != 1) ? arg_->info->error[1] : arg_->info->right[1];
      cv::Rect alertBox(box.x, box.y - alert.rows < 0 ? 0 : box.y - alert.rows, 
          alert.cols,  alert.rows);
      cv::Mat alertImage(arg_->info->m, alertBox);
      alert.copyTo(arg_->info->m, alertMask);
#endif
      cv::rectangle(arg_->info->m, box, scalar, 2, 1);
    }
  }
  arg_->info->videoCb->updateImage(arg_->info->m);
  std::lock_guard<std::mutex> guard(arg_->info->lock);
  arg_->info->done = true;
  arg_->info->cond.notify_one();
}

void HelmetTask::Run() {
  if (arg_->info->checkInfoList_.empty()) {
    return;
  }
  int inx = arg_->personIndex;
  auto checkInfo = arg_->info->checkInfoList_[inx];
  int ret = checkHelmet(checkInfo->personImage, checkInfo->result);
  if (ret < 0) {
    checkInfo->result.errorCode = -1;
  }
  if (1 == arg_->info->number_.fetch_add(-1)) {
    doDrawWork();
  }
}

int HelmetTask::checkHelmet(const cv::Mat &detectImage, HelmetCheckResult &result) {
  std::vector<unsigned char> faceImageData;
  cv::imencode(".jpg", detectImage, faceImageData);
  std::string imageBase64;
  Base64::getBase64().encode(faceImageData, imageBase64);
  ApiWrapper<HelmetClientDelegation> wrapper(*arg_->info->clients);
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
