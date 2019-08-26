#include "helmetClient.h"
#include <sstream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
using  apache::thrift::protocol::TProtocol;
using  apache::thrift::protocol::TBinaryProtocol;
using  apache::thrift::transport::TTransport;
using  apache::thrift::transport::TSocket;
using  apache::thrift::transport::TTransportException;
using  apache::thrift::TException;
using  apache::thrift::transport::TBufferedTransport;

void HelmetClientDelegation::init(const kunyan::Config &config) {
  std::string server = config.get("helmet", "server");
  std::string portStr  = config.get("helmet", "port");
  std::stringstream ss;
  int port = 0;
  ss << portStr;
  ss >> port;
  server_ = server;
  port_ = port;
  initClient();
}

bool HelmetClientDelegation::initClient() {
  errorConnectCount_ = 0;
  errorTime_ = time(NULL);
  status_ = false;
  std::shared_ptr<TTransport> socket(new TSocket(server_, port_));
  std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  client_ = std::make_shared<HelmetClient>(protocol);
  try {
    transport->open();
  } catch (TTransportException &e) {
    LOG(ERROR) << e.what();
    return false;
  }
  status_ = true;
  return true;
}

std::shared_ptr<HelmetClient>  HelmetClientDelegation::client() {
  auto getClient = [this] {
    std::shared_ptr<HelmetClient> client = nullptr;
    try {
      client_->pingHelmet();
    } catch (TTransportException &e) {
      LOG(ERROR) << e.what();
      return client;
    } catch (TException &e) {
      LOG(ERROR) << e.what();
    }
    return client_;
  };
  auto client = getClient();
  if (client != nullptr) {
    return client;
  }
  initClient();
  return getClient();
}