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

void HelmetClientDelegation::init(int number, const kunyan::Config &config) {

}

void HelmetClientDelegation::init(
    const kunyan::Config &config,
    const std::string &tag) {
    int port = 0;
    std::stringstream ss;
    std::string server = config.get(tag, "server");
    std::string portStr  = config.get(tag, "port");
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
  ((TSocket*)socket.get())->setRecvTimeout(5000);
  std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  client_ = std::make_shared<HelmetClient>(protocol);
  try {
    transport->open();
  } catch (TTransportException &e) {
    LOG(ERROR) << e.what();
    return false;
  }
  status_ = transport->isOpen();
  return status_;
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
  auto client = status_ ? getClient() : nullptr;
  if (client != nullptr) {
    return client;
  }
  if (!initClient()) {
    return nullptr;
  }
  return getClient();
}
