#ifndef INCLUDE_FACE_LIB_H
#define INCLUDE_FACE_LIB_H
#include "zdb/zdb.h"
#include "emsEntity.h"
#include "glog/logging.h"
namespace kems {
template <class T>
class EmsMapper: public RowMapper<T> {
 public:
   int mapRow(ResultSet_T r,
   int i,
   T &t) {
      this->getString(r, "to", t.to);
      this->getInt(r, "id", t.id);
      this->getString(r, "abstract", t.abstract);
      this->getString(r, "title", t.title);
      this->getInt(r, "stamp", t.stamp);
      return 0;
   }
 };
template <class T>
class EmsUidMapper : public RowMapper<T> {
 public:
   int mapRow(ResultSet_T r,
       int i,
       T &t) {
      this->getString(r, "user_id", t);
      return 0;
   }
};

} // namespace kems
#endif
