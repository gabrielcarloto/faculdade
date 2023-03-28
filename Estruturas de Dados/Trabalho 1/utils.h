#pragma once
#include <functional>
#include <iostream>

namespace utils {
void readFile(std::string path,
              const std::function<void(std::string)> &forEachLineCallback);
} // namespace utils
