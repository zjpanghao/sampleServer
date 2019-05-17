#ifndef INCLUDE_HTTP_UTIL_H
#define INCLUDE_HTTP_UTIL_H
#include <map>
#include <string>
#include "evhtp/evhtp.h"

#define HTTP_RECV_BUF_SIZE 16*1024*1024
struct evhtp_request;
struct evhttp_request;
struct evbuffer;
#ifdef EVHTP
typedef evhtp_request http_request;
#else
typedef evhttp_request http_request;
#endif
std::string getBodyStr(http_request *req);
void sendResponse(int errorCode, 
    std::string msg,  
    http_request *&req, 
    evbuffer *&response);

void sendResponseResult(int errorCode, 
    std::string msg,
    const std::map<std::string, std::string> &paraMap,
    http_request *&req); 

struct HttpControl {
  std::string url;
  void (*cb)(http_request *reg, void *arg);
};
#endif
