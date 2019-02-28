#ifndef INCLUDE_FACE_REPO_H
#define INCLUDE_FACE_REPO_H
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
 private:
  std::shared_ptr<DBPool> pool_;
};
}
#endif
