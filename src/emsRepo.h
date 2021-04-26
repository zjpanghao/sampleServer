#ifndef INCLUDE_FACE_REPO_H
#define INCLUDE_FACE_REPO_H
#include <memory>
#include <string>
#include "db/dbpool.h"
#include "emsEntity.h"
namespace kems {
struct News;
class EmsRepo {
 public:
  EmsRepo(std::shared_ptr<DBPool> pool);
  std::shared_ptr<News> getLatestNews();
  int saveNews(const News &news);
  int updateStatus(int id);

 private:
  std::shared_ptr<DBPool> pool_;
};
}
#endif
