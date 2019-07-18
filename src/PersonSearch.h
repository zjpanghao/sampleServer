/**
 * Autogenerated by Thrift Compiler (0.13.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef PersonSearch_H
#define PersonSearch_H

#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include <memory>
#include "person_types.h"



#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning (disable : 4250 ) //inheriting methods via dominance 
#endif

class PersonSearchIf {
 public:
  virtual ~PersonSearchIf() {}
  virtual void search(PersonSearchResult& _return, const std::string& image) = 0;
};

class PersonSearchIfFactory {
 public:
  typedef PersonSearchIf Handler;

  virtual ~PersonSearchIfFactory() {}

  virtual PersonSearchIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(PersonSearchIf* /* handler */) = 0;
};

class PersonSearchIfSingletonFactory : virtual public PersonSearchIfFactory {
 public:
  PersonSearchIfSingletonFactory(const ::std::shared_ptr<PersonSearchIf>& iface) : iface_(iface) {}
  virtual ~PersonSearchIfSingletonFactory() {}

  virtual PersonSearchIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(PersonSearchIf* /* handler */) {}

 protected:
  ::std::shared_ptr<PersonSearchIf> iface_;
};

class PersonSearchNull : virtual public PersonSearchIf {
 public:
  virtual ~PersonSearchNull() {}
  void search(PersonSearchResult& /* _return */, const std::string& /* image */) {
    return;
  }
};

typedef struct _PersonSearch_search_args__isset {
  _PersonSearch_search_args__isset() : image(false) {}
  bool image :1;
} _PersonSearch_search_args__isset;

class PersonSearch_search_args {
 public:

  PersonSearch_search_args(const PersonSearch_search_args&);
  PersonSearch_search_args& operator=(const PersonSearch_search_args&);
  PersonSearch_search_args() : image() {
  }

  virtual ~PersonSearch_search_args() noexcept;
  std::string image;

  _PersonSearch_search_args__isset __isset;

  void __set_image(const std::string& val);

  bool operator == (const PersonSearch_search_args & rhs) const
  {
    if (!(image == rhs.image))
      return false;
    return true;
  }
  bool operator != (const PersonSearch_search_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PersonSearch_search_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class PersonSearch_search_pargs {
 public:


  virtual ~PersonSearch_search_pargs() noexcept;
  const std::string* image;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PersonSearch_search_result__isset {
  _PersonSearch_search_result__isset() : success(false) {}
  bool success :1;
} _PersonSearch_search_result__isset;

class PersonSearch_search_result {
 public:

  PersonSearch_search_result(const PersonSearch_search_result&);
  PersonSearch_search_result& operator=(const PersonSearch_search_result&);
  PersonSearch_search_result() {
  }

  virtual ~PersonSearch_search_result() noexcept;
  PersonSearchResult success;

  _PersonSearch_search_result__isset __isset;

  void __set_success(const PersonSearchResult& val);

  bool operator == (const PersonSearch_search_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const PersonSearch_search_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PersonSearch_search_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PersonSearch_search_presult__isset {
  _PersonSearch_search_presult__isset() : success(false) {}
  bool success :1;
} _PersonSearch_search_presult__isset;

class PersonSearch_search_presult {
 public:


  virtual ~PersonSearch_search_presult() noexcept;
  PersonSearchResult* success;

  _PersonSearch_search_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class PersonSearchClient : virtual public PersonSearchIf {
 public:
  PersonSearchClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  PersonSearchClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
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
  void search(PersonSearchResult& _return, const std::string& image);
  void send_search(const std::string& image);
  void recv_search(PersonSearchResult& _return);
 protected:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class PersonSearchProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  ::std::shared_ptr<PersonSearchIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (PersonSearchProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_search(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  PersonSearchProcessor(::std::shared_ptr<PersonSearchIf> iface) :
    iface_(iface) {
    processMap_["search"] = &PersonSearchProcessor::process_search;
  }

  virtual ~PersonSearchProcessor() {}
};

class PersonSearchProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  PersonSearchProcessorFactory(const ::std::shared_ptr< PersonSearchIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::std::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::std::shared_ptr< PersonSearchIfFactory > handlerFactory_;
};

class PersonSearchMultiface : virtual public PersonSearchIf {
 public:
  PersonSearchMultiface(std::vector<std::shared_ptr<PersonSearchIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~PersonSearchMultiface() {}
 protected:
  std::vector<std::shared_ptr<PersonSearchIf> > ifaces_;
  PersonSearchMultiface() {}
  void add(::std::shared_ptr<PersonSearchIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void search(PersonSearchResult& _return, const std::string& image) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->search(_return, image);
    }
    ifaces_[i]->search(_return, image);
    return;
  }

};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
class PersonSearchConcurrentClient : virtual public PersonSearchIf {
 public:
  PersonSearchConcurrentClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot, std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync) : sync_(sync)
{
    setProtocol(prot);
  }
  PersonSearchConcurrentClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot, std::shared_ptr<::apache::thrift::async::TConcurrentClientSyncInfo> sync) : sync_(sync)
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
  void search(PersonSearchResult& _return, const std::string& image);
  int32_t send_search(const std::string& image);
  void recv_search(PersonSearchResult& _return, const int32_t seqid);
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
