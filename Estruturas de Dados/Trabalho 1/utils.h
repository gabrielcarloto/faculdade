#pragma once
#include <functional>
#include <iostream>

#define abs(x) (x > 0 ? x : -x)

namespace utils {
void readFile(std::string path,
              const std::function<void(std::string)> &forEachLineCallback);

} // namespace utils
