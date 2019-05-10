#ifndef INCLUDE_RW_LOCK_H
#define INCLUDE_RW_LOCK_H
#include <glog/logging.h>
#include <json/json.h>
#include <type_traits>
class LockMethod {
  public:
    virtual void lock(pthread_rwlock_t *lock) = 0;
    virtual ~LockMethod() {
    }
};

class RLockMethod : public LockMethod {
  public:
  ~RLockMethod() {
    LOG(INFO) << "Release rlock";
  }
  void lock(pthread_rwlock_t *lock) override {
    pthread_rwlock_rdlock(lock);
  }
};

class WLockMethod : public LockMethod {
  public:
  ~WLockMethod() {
    LOG(INFO) << "Release wlock";
  }
  void lock(pthread_rwlock_t *lock) override {
    pthread_rwlock_wrlock(lock);
  }
};

class RWLockGuard {
  public:
    RWLockGuard(LockMethod &method, pthread_rwlock_t *lock):lock_(lock) {
      method.lock(lock);
    }

    ~RWLockGuard() {
      pthread_rwlock_unlock(lock_);
    }

  private:
    pthread_rwlock_t *lock_;
};

class Util {
 public:
 template <class T, class U>
 static void convert(const T &t, U &u) {
  std::stringstream ss;
  ss << t;
  ss >> u;
}
};

class JsonUtil {
  public:
    static void getJsonInt(const Json::Value &value, const std::string &key, int &t) {
      if (value.isNull() || !value[key].isInt()) {
        return;
      }
      t = value[key].asInt();
    }

    static void getJsonDouble(const Json::Value &value, const std::string &key, double &t) {
      if (value.isNull() || !value[key].isDouble()) {
        return;
      }
      t = value[key].asDouble();
    }

    static void getJsonString(const Json::Value &value, const std::string &key, std::string &t) {
      if (value.isNull() || !value[key].isString()) {
        return;
      }
      t = value[key].asString();
    }
};
#endif
