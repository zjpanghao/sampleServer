#ifndef INCLUDE_FACE_REPO_H
#define INCLUDE_FACE_REPO_H
#include <memory>
#include <string>
#include "db/dbpool.h"
#include "projecttestEntity.h"
namespace kprojecttest {
struct News;
class ProjecttestRepo {
 public:
  ProjecttestRepo(std::shared_ptr<DBPool> pool);
  std::shared_ptr<News> getLatestNews();
 private:
  std::shared_ptr<DBPool> pool_;
};
}
#endif
