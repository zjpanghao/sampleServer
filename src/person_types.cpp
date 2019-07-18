/**
 * Autogenerated by Thrift Compiler (0.13.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "person_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>




SearchResult::~SearchResult() noexcept {
}


void SearchResult::__set_score(const double val) {
  this->score = val;
}

void SearchResult::__set_x(const int32_t val) {
  this->x = val;
}

void SearchResult::__set_y(const int32_t val) {
  this->y = val;
}

void SearchResult::__set_width(const int32_t val) {
  this->width = val;
}

void SearchResult::__set_height(const int32_t val) {
  this->height = val;
}
std::ostream& operator<<(std::ostream& out, const SearchResult& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t SearchResult::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_DOUBLE) {
          xfer += iprot->readDouble(this->score);
          this->__isset.score = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->x);
          this->__isset.x = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->y);
          this->__isset.y = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->width);
          this->__isset.width = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->height);
          this->__isset.height = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t SearchResult::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("SearchResult");

  xfer += oprot->writeFieldBegin("score", ::apache::thrift::protocol::T_DOUBLE, 1);
  xfer += oprot->writeDouble(this->score);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("x", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32(this->x);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("y", ::apache::thrift::protocol::T_I32, 3);
  xfer += oprot->writeI32(this->y);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("width", ::apache::thrift::protocol::T_I32, 4);
  xfer += oprot->writeI32(this->width);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("height", ::apache::thrift::protocol::T_I32, 5);
  xfer += oprot->writeI32(this->height);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(SearchResult &a, SearchResult &b) {
  using ::std::swap;
  swap(a.score, b.score);
  swap(a.x, b.x);
  swap(a.y, b.y);
  swap(a.width, b.width);
  swap(a.height, b.height);
  swap(a.__isset, b.__isset);
}

SearchResult::SearchResult(const SearchResult& other0) {
  score = other0.score;
  x = other0.x;
  y = other0.y;
  width = other0.width;
  height = other0.height;
  __isset = other0.__isset;
}
SearchResult& SearchResult::operator=(const SearchResult& other1) {
  score = other1.score;
  x = other1.x;
  y = other1.y;
  width = other1.width;
  height = other1.height;
  __isset = other1.__isset;
  return *this;
}
void SearchResult::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "SearchResult(";
  out << "score=" << to_string(score);
  out << ", " << "x=" << to_string(x);
  out << ", " << "y=" << to_string(y);
  out << ", " << "width=" << to_string(width);
  out << ", " << "height=" << to_string(height);
  out << ")";
}


PersonSearchResult::~PersonSearchResult() noexcept {
}


void PersonSearchResult::__set_errorCode(const int32_t val) {
  this->errorCode = val;
}

void PersonSearchResult::__set_errorMsg(const std::string& val) {
  this->errorMsg = val;
}

void PersonSearchResult::__set_result(const std::vector<SearchResult> & val) {
  this->result = val;
}
std::ostream& operator<<(std::ostream& out, const PersonSearchResult& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t PersonSearchResult::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->errorCode);
          this->__isset.errorCode = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->errorMsg);
          this->__isset.errorMsg = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_LIST) {
          {
            this->result.clear();
            uint32_t _size2;
            ::apache::thrift::protocol::TType _etype5;
            xfer += iprot->readListBegin(_etype5, _size2);
            this->result.resize(_size2);
            uint32_t _i6;
            for (_i6 = 0; _i6 < _size2; ++_i6)
            {
              xfer += this->result[_i6].read(iprot);
            }
            xfer += iprot->readListEnd();
          }
          this->__isset.result = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t PersonSearchResult::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("PersonSearchResult");

  xfer += oprot->writeFieldBegin("errorCode", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(this->errorCode);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("errorMsg", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->errorMsg);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("result", ::apache::thrift::protocol::T_LIST, 3);
  {
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_STRUCT, static_cast<uint32_t>(this->result.size()));
    std::vector<SearchResult> ::const_iterator _iter7;
    for (_iter7 = this->result.begin(); _iter7 != this->result.end(); ++_iter7)
    {
      xfer += (*_iter7).write(oprot);
    }
    xfer += oprot->writeListEnd();
  }
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(PersonSearchResult &a, PersonSearchResult &b) {
  using ::std::swap;
  swap(a.errorCode, b.errorCode);
  swap(a.errorMsg, b.errorMsg);
  swap(a.result, b.result);
  swap(a.__isset, b.__isset);
}

PersonSearchResult::PersonSearchResult(const PersonSearchResult& other8) {
  errorCode = other8.errorCode;
  errorMsg = other8.errorMsg;
  result = other8.result;
  __isset = other8.__isset;
}
PersonSearchResult& PersonSearchResult::operator=(const PersonSearchResult& other9) {
  errorCode = other9.errorCode;
  errorMsg = other9.errorMsg;
  result = other9.result;
  __isset = other9.__isset;
  return *this;
}
void PersonSearchResult::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "PersonSearchResult(";
  out << "errorCode=" << to_string(errorCode);
  out << ", " << "errorMsg=" << to_string(errorMsg);
  out << ", " << "result=" << to_string(result);
  out << ")";
}


