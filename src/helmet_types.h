/**
 * Autogenerated by Thrift Compiler (0.13.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef helmet_TYPES_H
#define helmet_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <functional>
#include <memory>




class HelmetCheckResult;

typedef struct _HelmetCheckResult__isset {
  _HelmetCheckResult__isset() : errorCode(false), index(false), name(false), score(false) {}
  bool errorCode :1;
  bool index :1;
  bool name :1;
  bool score :1;
} _HelmetCheckResult__isset;

class HelmetCheckResult : public virtual ::apache::thrift::TBase {
 public:

  HelmetCheckResult(const HelmetCheckResult&);
  HelmetCheckResult& operator=(const HelmetCheckResult&);
  HelmetCheckResult() : errorCode(0), index(0), name(), score(0) {
  }

  virtual ~HelmetCheckResult() noexcept;
  int32_t errorCode;
  int32_t index;
  std::string name;
  double score;

  _HelmetCheckResult__isset __isset;

  void __set_errorCode(const int32_t val);

  void __set_index(const int32_t val);

  void __set_name(const std::string& val);

  void __set_score(const double val);

  bool operator == (const HelmetCheckResult & rhs) const
  {
    if (!(errorCode == rhs.errorCode))
      return false;
    if (!(index == rhs.index))
      return false;
    if (!(name == rhs.name))
      return false;
    if (!(score == rhs.score))
      return false;
    return true;
  }
  bool operator != (const HelmetCheckResult &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const HelmetCheckResult & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(HelmetCheckResult &a, HelmetCheckResult &b);

std::ostream& operator<<(std::ostream& out, const HelmetCheckResult& obj);



#endif
