#pragma once
#include <functional>
#include <iostream>
#include <stdint.h>

template <typename TItem, typename TReturn = void>
using ItemIndexCallback =
    const std::function<TReturn(TItem &item, const size_t index)> &;

template <typename T> class BaseList {
private:
  void throwOutOfRange(size_t index);
  void throwOutOfRange(intmax_t index);
  void (*itemReleaseCallback)(T &) = NULL;

protected:
  size_t length;

  bool checkReleaseCallback() { return itemReleaseCallback != NULL; };

  void callReleaseCallback(T &);
  void rawCallReleaseCallback(T &);
  void assertIndexIsValid(size_t index);
  void assertIndexIsValid(intmax_t index);

public:
  ~BaseList() = default;

  void registerItemReleaseCallback(void (*fn)(T &));
  void registerItemReleaseCallback();

  virtual T &operator[](size_t index) = 0;
  size_t getLength();

  virtual T &at(intmax_t index) = 0;
  virtual void push(const T &item) = 0;
  virtual void remove(size_t index) = 0;
  virtual void insert(T item, size_t index = 0) = 0;
  virtual void replace(T item, size_t index = 0) = 0;
  // virtual void slice(size_t indexStart, size_t indexEnd = NULL);
  virtual BaseList<T> &filter(ItemIndexCallback<T, bool> filterFn) = 0;
  virtual bool find(ItemIndexCallback<T, bool> filterFn, T &item) = 0;
  virtual void forEach(ItemIndexCallback<T> callback,
                       size_t startIndex = 0) = 0;

  virtual bool findIndex(ItemIndexCallback<T, bool> filterFn,
                         size_t &index) = 0;
};

template <typename T> void BaseList<T>::assertIndexIsValid(size_t index) {
  if (index <= length)
    return;

  throwOutOfRange(index);
}

template <typename T> void BaseList<T>::assertIndexIsValid(intmax_t index) {
  if (index <= length && index >= -static_cast<intmax_t>(length))
    return;

  throwOutOfRange(index);
}

template <typename T> void BaseList<T>::throwOutOfRange(size_t index) {
  char message[100];
  snprintf(message, 100, "Index out of range: %zu", index);

  throw std::out_of_range(message);
}

template <typename T> void BaseList<T>::throwOutOfRange(intmax_t index) {
  char message[100];
  snprintf(message, 100, "Index out of range: %lld", index);

  throw std::out_of_range(message);
}

template <typename T>
void BaseList<T>::registerItemReleaseCallback(void (*fn)(T &)) {
  itemReleaseCallback = fn;
}

template <typename T> size_t BaseList<T>::getLength() { return length; }

template <typename T> void defaultItemRelease(T &item) { delete item; }

template <typename T> void BaseList<T>::registerItemReleaseCallback() {
  itemReleaseCallback = defaultItemRelease;
}

template <typename T> void BaseList<T>::callReleaseCallback(T &item) {
  if (itemReleaseCallback != NULL)
    (*itemReleaseCallback)(item);
}

template <typename T> void BaseList<T>::rawCallReleaseCallback(T &item) {
  (*itemReleaseCallback)(item);
}
