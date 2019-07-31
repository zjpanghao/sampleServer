#include "detectService.h"
#include <sys/time.h>
#include <set>
#include <glog/logging.h>
#include <regex>
#include <iterator>
#include "util.h"
#include "faceConst.h"
#include "blas.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
extern int gpu_index;
namespace kface {
DetectService& DetectService::getDetectService() {
  static DetectService detectService;
  return detectService;
}

DetectService::DetectService() {
}

int DetectService::init(const kunyan::Config &config) {
  LOG(INFO) << "init detect service";
#ifdef GPU
  gpu_index = 0;
  cuda_set_device(gpu_index);
  LOG(INFO) << "cuda set device ok";
#endif
  char network[512]={0};
  strncpy(network, config.get("cfg", "network").c_str(), sizeof(network));
  char weights[512]={0};
  strncpy(weights, config.get("cfg", "weights").c_str(), sizeof(weights));
  LOG(INFO) << network;
  LOG(INFO) << weights;
  char name[512]= {0};
  strncpy(name, config.get("cfg", "name").c_str(), sizeof(name));
  list *options = read_data_cfg(name);
  LOG(INFO) << name;
  char *name_list = option_find_str(options, "names", "data/names.list");
  names_ = get_labels(name_list);
  //image **alphabet = load_alphabet();
  net_ = load_network(network,
      weights,
      0);
  if (net_) {
    set_batch_network(net_, 1);
    LOG(INFO) << "net init:" << net_->w << " ," << net_->h <<"n is:" <<  net_->n;
  }
  return 0;
}

static image maToImage(const cv::Mat &src) {
  int h = src.rows;
  int w = src.cols;
  int c = src.channels();
  image im = make_image(w, h, c);
  unsigned char *data = src.data;
  int step = 3 * src.cols;
  int i, j, k;

  for(i = 0; i < h; ++i){
    for(k= 0; k < c; ++k){
            for(j = 0; j < w; ++j){
                im.data[k*w*h + i*w + j] = data[i*step + j*c + k]/255.;
            }
        }
    }
    return im;
}

int DetectService::getDetectResult(const cv::Mat &m, 
    std::vector<ObjectDetectResult> &result) {
  int rc = 0;
#if 1
  //cv::Mat m = cv::imdecode(data, cv::IMREAD_COLOR);
  image im = maToImage(m);

  rgbgr_image(im);
  if (net_ != NULL) {
   // LOG(INFO) << "before predict" << net_->w << " ," << net_->h <<"n is:" <<  net_->n;
  }else {
    LOG(INFO) << "THE net is null";
    return -1;
  }
  image sized = letterbox_image(im, net_->w, net_->h);
  layer l = net_->layers[net_->n-1];
  float *X = sized.data;
  network_predict(net_, X);
  int nboxes = 0;
  float nms = 0.45;
  float thresh = 0.4;
  detection *dets = get_network_boxes(net_, im.w, im.h, thresh, 0.5, 0, 1, &nboxes);
  //LOG(INFO) << "The box is :" << nboxes << "classes is:" << l.classes;

  if (nms) do_nms_sort(dets, nboxes, l.classes, nms);
  for(int i = 0; i < nboxes; ++i){
    for(int j = 0; j < l.classes; ++j){
      if (dets[i].prob[j] > thresh){
        ObjectDetectResult detectResult;
        detectResult.category = names_[j];
        detectResult.score = dets[i].prob[j]*100;
        auto box = dets[i].bbox;
        detectResult.x = (box.x - box.w / 2) * m.cols;
        detectResult.y = (box.y - box.h / 2) * m.rows;
        detectResult.width = box.w * m.cols;
        detectResult.height = box.h * m.rows;
        result.push_back(detectResult);
      }
    }
  }
  free_detections(dets, nboxes);
  free_image(im);
  free_image(sized);
#endif
  return rc;
}
}
