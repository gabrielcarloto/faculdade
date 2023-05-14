#ifdef _DEBUG
#include <cstdlib>
#include <iostream>
unsigned int count = 0;

void *operator new(size_t size) {
  count++;
  void *ptr = malloc(size);
  return ptr;
}

void operator delete(void *ptr) {
  count--;
  free(ptr);
}

void operator delete(void *ptr, size_t) {
  count--;
  free(ptr);
}

void checkCountDifference() {
  if (count > 0)
    std::cout << "Memory leak detected. new/delete calls difference: " << count
              << '\n';
}

namespace {
struct RegisterDebugFeatures {
  RegisterDebugFeatures() { atexit(checkCountDifference); }
};

static RegisterDebugFeatures registerDebugFeatures;
} // namespace
#endif
