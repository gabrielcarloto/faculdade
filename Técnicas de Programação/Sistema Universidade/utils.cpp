#include <array>
#include <iostream>
#include <random>
#include <sstream>

namespace utils {
// https://stackoverflow.com/a/58467162
auto get_uuid() -> std::string {
  static std::random_device dev;
  static std::mt19937 rng(dev());

  std::uniform_int_distribution<int> dist(0, 15);

  const char *v = "0123456789abcdef";
  const std::array<bool, 16> dash = {false, false, false, false, true, false,
                                     true,  false, true,  false, true, false,
                                     false, false, false, false};

  std::string res;

  for (int i = 0; i < 16; i++) {
    if (dash[i])
      res += "-";
    res += v[dist(rng)];
    res += v[dist(rng)];
  }

  return res;
}
} // namespace utils
