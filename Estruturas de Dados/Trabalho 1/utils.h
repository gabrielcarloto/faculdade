#pragma once
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

#ifdef _DEBUG
#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define absolute(x) (x > 0 ? x : -x)

template <typename T> class Vector;

struct Person {
  std::string name;
  unsigned int id = 0;
};

namespace utils {
void readFile(std::string path,
              const std::function<void(std::string)> &forEachLineCallback);

void writeFile(std::string path, std::string content);

Vector<char *> splitStr(char *str, const char *delimiter);

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
