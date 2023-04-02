#pragma once
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

#define absolute(x) (x > 0 ? x : -x)

namespace utils {
void readFile(std::string path,
              const std::function<void(std::string)> &forEachLineCallback);

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
