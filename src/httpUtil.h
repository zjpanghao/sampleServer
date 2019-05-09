#ifndef INCLUDE_HTTP_UTIL_H
#define INCLUDE_HTTP_UTIL_H
#include <map>
#include <string>
#include "evhtp/evhtp.h"

#define HTTP_RECV_BUF_SIZE 500*1024
struct evhtp_request;
struct evbuffer;
std::string getBodyStr(struct evhtp_request *req);
void sendResponse(int errorCode, 
    std::string msg,  
    struct evhtp_request *&req, 
    evbuffer *&response);

template<class vvalue>
void sendResponseResult(int errorCode, 
    std::string msg,
    const std::map<std::string, vvalue> &paraMap,
    struct evhtp_request *&req, 
    evbuffer *&response); 

struct HttpControl {
  std::string url;
  void (*cb)(evhtp_request_t *reg, void *arg);
};

#endif
