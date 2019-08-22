#include "detectServiceCvImpl.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "util.h"
#include "faceConst.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
namespace ktrack {

int DetectServiceCvImpl::init(const kunyan::Config &config) {
  LOG(INFO) << "init detect service";
  net_ = cv::dnn::readNetFromDarknet(config.get("cfg", "network"), 
                           config.get("cfg", "weights"));
  net_.setPreferableBackend(cv::dnn::Backend::DNN_BACKEND_OPENCV);
  net_.setPreferableTarget(cv::dnn::Target::DNN_TARGET_CPU);
  return 0;
}

static std::vector<std::string> getOutputsNames(const cv::dnn::Net& net) {
    std::vector<std::string> names;
    std::vector<int> outLayers = net.getUnconnectedOutLayers();
    std::vector<std::string> layersNames = net.getLayerNames();
    names.resize(outLayers.size());
    for (size_t i = 0; i < outLayers.size(); ++i)
          names[i] = layersNames[outLayers[i] - 1];
    return names;
}

static void resetBox(ObjectDetectResult &result, const cv::Mat &m) {
  if (result.x < 0) {
    result.x = 0;
  }
  if (result.y < 0) {
    result.y = 0;
  }
  if (result.x > m.cols) {
    result.x = m.cols;
  }
  if (result.y > m.rows) {
    result.y = m.rows;
  }
  if (result.x + result.width > m.cols) {
    result.width = m.cols - result.x;
  }
  if (result.height + result.y > m.rows) {
    result.height = m.rows - result.y;
  }
}

int DetectServiceCvImpl::getDetectResult(const cv::Mat &m, 
    std::vector<ObjectDetectResult> &results) {
  int rc = 0;
  int confThreshold = 0.3;
  cv::Mat inp = cv::dnn::blobFromImage(m, 1.0/255, cv::Size(416, 416), cv::Scalar(), true, false);
  net_.setInput(inp);
  std::vector<cv::Mat> outs;
  net_.forward(outs, getOutputsNames(net_));
  //auto out = net_.forward();
  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;
  for (auto &out : outs) {
    for (int i = 0; i < out.rows; i++) {
      cv::Mat scores = out.row(i).colRange(5, out.cols);
      double confidence;
      cv::Point maxLoc;
      cv::minMaxLoc(scores, 0, &confidence, 0, &maxLoc);
      if (confidence > confThreshold) {
        float* detection = out.ptr<float>(i);
        double centerX = detection[0];
        double centerY = detection[1];
        double width = detection[2];
        double height = detection[3];
        classIds.push_back(maxLoc.x);
        confidences.push_back((float)confidence);
        ObjectDetectResult result;
        result.category = "person";
        int x = m.cols * (centerX - width / 2);
        int y = m.rows * (centerY - height / 2);
        int rWidth = m.cols*(width);
        int rHeight = m.rows*(height);
        boxes.push_back(cv::Rect(x, y, rWidth, rHeight));
      }
    }
  }
  // here we need NMS of boxes
  std::vector<int> indices;
  float nmsThreshold = 0.1;
  cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
  for (auto inx : indices) {
    if (classIds[inx] != 0) {
      continue;
    }
    ObjectDetectResult result;
    result.category = classIds[inx] == 0 ? "person" : "null";
    result.score = confidences[inx];
    result.x = boxes[inx].x;
    result.y = boxes[inx].y;
    result.width = boxes[inx].width;
    result.height = boxes[inx].height;
    resetBox(result, m);
    results.push_back(result);
  }

  return rc;
}

}
