#ifndef INCLUDE_FACE_REPO_H
#define INCLUDE_FACE_REPO_H
#include <list>
#include <memory>
#include <string>
#include "db/dbpool.h"
#include "faceEntity.h"
namespace kface {
struct News;
class FaceRepo {
 public:
  FaceRepo(std::shared_ptr<DBPool> pool);
  std::shared_ptr<News> getLatestNews();
  std::list<std::shared_ptr<News>> getNews(int page, int size);
  std::list<std::shared_ptr<News>> getNewsSmallThan(int id, int size);
  std::list<std::shared_ptr<News>> getNewsInfoSmallThan(int id, int size);
  int saveNews(std::shared_ptr<News> news); 
  int getImage(int imageId, std::string &image);
 private:
  std::shared_ptr<DBPool> pool_;
};

}
#endif
