#ifndef INCLUDE_FACE_LIB_H
#define INCLUDE_FACE_LIB_H
#include "zdb/zdb.h"
#include "projecttestEntity.h"
#include "glog/logging.h"
namespace kprojecttest {
template <class T>
class ProjecttestUidMapper : public RowMapper<T> {
 public:
   int mapRow(ResultSet_T r,
       int i,
       T &t) {
      this->getString(r, "user_id", t);
      return 0;
   }
};

} // namespace kprojecttest
#endif
