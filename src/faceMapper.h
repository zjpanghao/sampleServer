#ifndef INCLUDE_FACE_LIB_H
#define INCLUDE_FACE_LIB_H
#include "zdb/zdb.h"
#include "faceEntity.h"
#include "glog/logging.h"
namespace kface {
template <class T>
class FaceUidMapper : public RowMapper<T> {
 public:
   int mapRow(ResultSet_T r,
       int i,
       T &t) {
      this->getString(r, "user_id", t);
      return 0;
   }
};

} // namespace kface
#endif
