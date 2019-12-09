#include "trackDraw.h"
#include <glog/logging.h>

void TrackDraw::drawDouble(cv::Mat &m,
    double score, int x, int y,
    cv::Scalar &scalar) {
    std::stringstream costTime;
    costTime.precision(2);
    costTime.setf(std::ios::fixed);
    costTime << score;
    cv::putText(m, costTime.str(), cv::Point(x, y),
        cv::FONT_HERSHEY_PLAIN, 5, scalar,3);

}

void TrackDraw::doDrawWork(int trackId, const cv::Mat &bgmask, const std::vector<ktrack::DetectInfo> &results, cv::Mat &mo) {
  // landmark
  for (auto &result : results) {
    cv::Mat cutMask(bgmask, result.helmet.getRect());
    std::stringstream ss;
    static int inx = 0;
    inx = (inx + 1) % 100;
    ss << "landmark" <<trackId << "/" 
      << "record_" << inx << ".jpg";
    cv::Mat cut(mo,result.helmet.getRect());
    cv::imwrite(ss.str().c_str(), cut);
    ss.str("");
    ss.clear();
    ss << "mask" <<trackId << "/" << inx << ".jpg";
    cv::imwrite(ss.str().c_str(), cutMask);
  }
  int beginx = 0;
  for (auto &result : results) {
    static cv::Scalar BLUE = cv::Scalar(255, 0, 0);
    static cv::Scalar GREEN = cv::Scalar(0, 255, 0);
    static cv::Scalar RED = cv::Scalar(0, 0, 255);
    cv::Scalar scalar;
    std::string category = result.helmet.category;
    LOG(INFO) << "person" << "score:" << result.helmet.score;
    if (category == "landmark") {
      scalar = GREEN;  
    } else if (category == "with") {
      scalar = BLUE;
    } else {
      scalar = RED;
    }
    std::stringstream scoreStr;
    scoreStr << result.helmet.score;
    drawDouble(mo,result.helmet.score,
      result.person.x + 30, 
      result.person.y,
      scalar);
    drawDouble(mo,result.person.costTime,
      result.person.x + 30, 
      result.person.y + 50,
      scalar);

    drawDouble(mo,result.face.costTime,
      result.person.x + 30, 
      result.person.y + 100,
      scalar);
    drawDouble(mo,result.helmet.costTime,
      result.person.x + 30, 
      result.person.y + 150,
      scalar);

    cv::rectangle(mo,result.helmet.getRect(), scalar, 2, 1);
    cv::rectangle(mo, result.person.getRect(), cv::Scalar(255,255,255), 2, 1);
    if (result.face.valid) {
      cv::rectangle(mo,result.face.getRect(), cv::Scalar(0,255,255), 2, 1);
    }
    cv::Mat helmetMask(bgmask, result.helmet.getRect());
    cv::cvtColor(helmetMask, helmetMask,
        cv::COLOR_GRAY2BGR);
    if (beginx < mo.cols - result.helmet.width) {
      cv::Rect maskViewRect(beginx, 0, result.helmet.width, result.helmet.height);
      beginx += result.helmet.width + 50;
      cv::Mat maskView(mo, maskViewRect);
      helmetMask.copyTo(maskView);
    }
  }
  static int inx = 0;
  inx = (inx + 1) % 100;
  std::stringstream ss;
  ss << "cv" <<trackId << "/" << inx << ".jpg";
  cv::imwrite(ss.str().c_str(), mo);
  //videoInfo_->updateImage(mo);
}

