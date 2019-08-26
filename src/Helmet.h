/**
 * Autogenerated by Thrift Compiler (0.13.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef Helmet_H
#define Helmet_H

#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include <memory>
#include "helmet_types.h"



#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning (disable : 4250 ) //inheriting methods via dominance 
#endif

class HelmetIf {
 public:
  virtual ~HelmetIf() {}
  virtual void checkHelmet(HelmetCheckResult& _return, const std::string& image) = 0;
  virtual void pingHelmet() = 0;
};

class HelmetIfFactory {
 public:
  typedef HelmetIf Handler;

  virtual ~HelmetIfFactory() {}

  virtual HelmetIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(HelmetIf* /* handler */) = 0;
};

class HelmetIfSingletonFactory : virtual public HelmetIfFactory {
 public:
  HelmetIfSingletonFactory(const ::std::shared_ptr<HelmetIf>& iface) : iface_(iface) {}
  virtual ~HelmetIfSingletonFactory() {}

  virtual HelmetIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(HelmetIf* /* handler */) {}

 protected:
  ::std::shared_ptr<HelmetIf> iface_;
};

class HelmetNull : virtual public HelmetIf {
 public:
  virtual ~HelmetNull() {}
  void checkHelmet(HelmetCheckResult& /* _return */, const std::string& /* image */) {
    return;
  }
  void pingHelmet() {
    return;
  }
};

typedef struct _Helmet_checkHelmet_args__isset {
  _Helmet_checkHelmet_args__isset() : image(false) {}
  bool image :1;
} _Helmet_checkHelmet_args__isset;

class Helmet_checkHelmet_args {
 public:

  Helmet_checkHelmet_args(const Helmet_checkHelmet_args&);
  Helmet_checkHelmet_args& operator=(const Helmet_checkHelmet_args&);
  Helmet_checkHelmet_args() : image() {
  }

  virtual ~Helmet_checkHelmet_args() noexcept;
  std::string image;

  _Helmet_checkHelmet_args__isset __isset;

  void __set_image(const std::string& val);

  bool operator == (const Helmet_checkHelmet_args & rhs) const
  {
    if (!(image == rhs.image))
      return false;
    return true;
  }
  bool operator != (const Helmet_checkHelmet_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Helmet_checkHelmet_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Helmet_checkHelmet_pargs {
 public:


  virtual ~Helmet_checkHelmet_pargs() noexcept;
  const std::string* image;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Helmet_checkHelmet_result__isset {
  _Helmet_checkHelmet_result__isset() : success(false) {}
  bool success :1;
} _Helmet_checkHelmet_result__isset;

class Helmet_checkHelmet_result {
 public:

  Helmet_checkHelmet_result(const Helmet_checkHelmet_result&);
  Helmet_checkHelmet_result& operator=(const Helmet_checkHelmet_result&);
  Helmet_checkHelmet_result() {
  }

  virtual ~Helmet_checkHelmet_result() noexcept;
  HelmetCheckResult success;

  _Helmet_checkHelmet_result__isset __isset;

  void __set_success(const HelmetCheckResult& val);

  bool operator == (const Helmet_checkHelmet_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Helmet_checkHelmet_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Helmet_checkHelmet_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Helmet_checkHelmet_presult__isset {
  _Helmet_checkHelmet_presult__isset() : success(false) {}
  bool success :1;
} _Helmet_checkHelmet_presult__isset;

class Helmet_checkHelmet_presult {
 public:


  virtual ~Helmet_checkHelmet_presult() noexcept;
  HelmetCheckResult* success;

  _Helmet_checkHelmet_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class Helmet_pingHelmet_args {
 public:

  Helmet_pingHelmet_args(const Helmet_pingHelmet_args&);
  Helmet_pingHelmet_args& operator=(const Helmet_pingHelmet_args&);
  Helmet_pingHelmet_args() {
  }

  virtual ~Helmet_pingHelmet_args() noexcept;

  bool operator == (const Helmet_pingHelmet_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const Helmet_pingHelmet_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Helmet_pingHelmet_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Helmet_pingHelmet_pargs {
 public:


  virtual ~Helmet_pingHelmet_pargs() noexcept;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Helmet_pingHelmet_result {
 public:

  Helmet_pingHelmet_result(const Helmet_pingHelmet_result&);
  Helmet_pingHelmet_result& operator=(const Helmet_pingHelmet_result&);
  Helmet_pingHelmet_result() {
  }

  virtual ~Helmet_pingHelmet_result() noexcept;

  bool operator == (const Helmet_pingHelmet_result & /* rhs */) const
  {
    return true;
  }
  bool operator != (const Helmet_pingHelmet_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Helmet_pingHelmet_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Helmet_pingHelmet_presult {
 public:


  virtual ~Helmet_pingHelmet_presult() noexcept;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class HelmetClient : virtual public HelmetIf {
 public:
  HelmetClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  HelmetClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void checkHelmet(HelmetCheckResult& _return, const std::string& image);
  void send_checkHelmet(const std::string& image);
  void recv_checkHelmet(HelmetCheckResult& _return);
  void pingHelmet();
  void send_pingHelmet();
  void recv_pingHelmet();
 protected:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class HelmetProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  ::std::shared_ptr<HelmetIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (HelmetProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_checkHelmet(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_pingHelmet(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  HelmetProcessor(::std::shared_ptr<HelmetIf> iface) :
    iface_(iface) {
    processMap_["checkHelmet"] = &HelmetProcessor::process_checkHelmet;
    processMap_["pingHelmet"] = &HelmetProcessor::process_pingHelmet;
  }

  virtual ~HelmetProcessor() {}
};

class HelmetProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  HelmetProcessorFactory(const ::std::shared_ptr< HelmetIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::std::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::std::shared_ptr< HelmetIfFactory > handlerFactory_;
};

class HelmetMultiface : virtual public HelmetIf {
 public:
  HelmetMultiface(std::vector<std::shared_ptr<HelmetIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~HelmetMultiface() {}
 protected:
  std::vector<std::shared_ptr<HelmetIf> > ifaces_;
  HelmetMultiface() {}
  void add(::std::shared_ptr<HelmetIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void checkHelmet(HelmetCheckResult& _return, const std::string& image) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->checkHelmet(_return, image);
    }
    ifaces_[i]->checkHelmet(_return, image);
    return;
  }

  void pingHelmet() {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->pingHelmet();
    }
    ifaces_[i]->pingHelmet();
  }

};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
class HelmetConcurrentClient : virtual public HelmetIf {
 public:
  HelmetConcurrentClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot, std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync) : sync_(sync)
{
    setProtocol(prot);
  }
  HelmetConcurrentClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot, std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync) : sync_(sync)
{
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void checkHelmet(HelmetCheckResult& _return, const std::string& image);
  int32_t send_checkHelmet(const std::string& image);
  void recv_checkHelmet(HelmetCheckResult& _return, const int32_t seqid);
  void pingHelmet();
  int32_t send_pingHelmet();
  void recv_pingHelmet(const int32_t seqid);
 protected:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
  std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync_;
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif



#endif