#pragma once
#include "debug.h"
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

#define absolute(x) (x > 0 ? x : -x)

template <typename T> class Vector;

struct Person {
  std::string name;
  unsigned int id = 0;

  bool operator<(Person &p) { return id < p.id; }
  bool operator<=(Person &p) { return id <= p.id; }
  bool operator>(Person &p) { return id > p.id; }
  bool operator>=(Person &p) { return id >= p.id; }
  bool operator==(Person &p) { return id == p.id; }
};

namespace utils {
void readFile(std::string path,
              const std::function<void(std::string)> &forEachLineCallback);

void writeFile(std::string path, std::string content);

Vector<char *> splitStr(char *str, const char *delimiter, size_t reserve = 0);

Person *strToPerson(std::string &str);
std::string personToString(Person *);

bool verifyString(std::string &);

// https://stackoverflow.com/a/56268886
template <typename TF, typename TDuration, class... TArgs>
std::result_of_t<TF && (TArgs && ...)>
callWithTimeout(TF &&f, TDuration timeout, TArgs &&...args) {
  using R = std::result_of_t<TF && (TArgs && ...)>;
  std::packaged_task<R(TArgs...)> task(f);
  auto future = task.get_future();
  std::thread thr(std::move(task), std::forward<TArgs>(args)...);
  if (future.wait_for(timeout) != std::future_status::timeout) {
    thr.join();
    return future.get(); // this will propagate exception from f() if any
  } else {
    thr.detach(); // we leave the thread still running
    throw std::runtime_error("Timeout");
  }
}
} // namespace utils
