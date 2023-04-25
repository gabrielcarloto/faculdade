#pragma once
#include "Profiler.h"
#include <cstdint>
#include <functional>
#include <iostream>

template <typename TItem, typename TReturn = void>
using ItemIndexCallback =
    const std::function<TReturn(TItem &item, const size_t index)> &;

template <typename T> void defaultItemRelease(T &item);

template <typename T, class Derived, class Iterator> class BaseList {
private:
  void throwOutOfRange(size_t index) {
    char message[100];
    snprintf(message, 100, "Index out of range: %zu", index);

    throw std::out_of_range(message);
  };

  void throwOutOfRange(intmax_t index) {
    char message[100];
    snprintf(message, 100, "Index out of range: %lld", index);

    throw std::out_of_range(message);
  };

  std::function<void(T &)> itemReleaseCallback = nullptr;

protected:
  size_t length;
  Profiler profiler;

  bool checkReleaseCallback() { return itemReleaseCallback != nullptr; };

  void callReleaseCallback(T &item) {
    profiler.addComparison();
    if (itemReleaseCallback != nullptr)
      itemReleaseCallback(item);
  };

  void rawCallReleaseCallback(T &item) { itemReleaseCallback(item); };

  size_t intmax_t_to_size_t(intmax_t size) {
    return size >= 0 ? size : length + size;
  }

  void assertIndexIsValid(size_t index) {
    profiler.addComparison();
    if (index < length)
      return;

    throwOutOfRange(index);
  };

  void assertIndexIsValid(intmax_t index) {
    profiler.addComparison();
    if (index < static_cast<intmax_t>(length) &&
        static_cast<intmax_t>(length) + index > 0)
      return;

    throwOutOfRange(index);
  };

  virtual T &_at(intmax_t index) = 0;
  virtual void _push(const T &item) = 0;
  virtual void _push(const T &&item) = 0;
  virtual void _remove(size_t index) = 0;
  virtual void _insert(T item, size_t index = 0) = 0;
  virtual void _replace(T item, size_t index = 0) = 0;
  // virtual void slice(size_t indexStart, size_t indexEnd = nullptr);
  virtual Derived _filter(ItemIndexCallback<T, bool> filterFn) = 0;
  virtual bool _find(ItemIndexCallback<T, bool> filterFn, T &item) = 0;
  virtual void _forEach(ItemIndexCallback<T> callback,
                        size_t startIndex = 0) = 0;

  virtual bool _findIndex(ItemIndexCallback<T, bool> filterFn,
                          size_t &index) = 0;

public:
  BaseList(const size_t length) : profiler() { this->length = length; }
  ~BaseList() = default;

  void registerItemReleaseCallback(const std::function<void(T &)> &fn) {
    itemReleaseCallback = std::move(fn);
  };

  void registerItemReleaseCallback() {
    itemReleaseCallback = defaultItemRelease;
  };

  virtual T &operator[](size_t index) = 0;
  size_t getLength() { return length; };

  T &at(intmax_t index) {
    profiler.start();
    assertIndexIsValid(index);
    T &item = _at(index);
    profiler.end();
    return item;
  };

  void push(const T &item) { _push(item); };
  void push(const T &&item) { _push(item); };

  void remove(size_t index) {
    profiler.start();
    assertIndexIsValid(index);
    _remove(index);
    profiler.end();
  };

  void insert(const T &item, size_t index = 0) {
    profiler.start();
    profiler.addComparison(2);

    if (index == length) {
      _push(item);
      return profiler.end();
    } else if (index == length - 1) {
      assertIndexIsValid(index);
      T &lastItem = _at(index);
      _replace(item, index);
      _push(lastItem);
      return profiler.end();
    }

    assertIndexIsValid(index);

    _insert(item, index);
    profiler.end();
  };

  void insert(const T &item, intmax_t index) {
    profiler.start();
    profiler.addComparison(2);
    auto i = intmax_t_to_size_t(index);

    if (i == length) {
      _push(item);
      return profiler.end();
    } else if (i == length - 1) {
      assertIndexIsValid(index);
      T &lastItem = _at(i);
      _replace(item, i);
      _push(lastItem);
      return profiler.end();
    }

    assertIndexIsValid(index);

    _insert(item, i);
    profiler.end();
  }

  void replace(T item, size_t index = 0) {
    profiler.start();
    assertIndexIsValid(index);
    _replace(item, index);
    profiler.end();
  };

  Derived filter(ItemIndexCallback<T, bool> filterFn) {
    profiler.start();
    Derived list = _filter(filterFn);
    profiler.end();
    return list;
  };

  bool find(ItemIndexCallback<T, bool> filterFn, T &item) {
    profiler.start();
    auto found = _find(filterFn, item);
    profiler.end();
    return found;
  };

  void forEach(ItemIndexCallback<T> callback, size_t startIndex = 0) {
    profiler.start();
    assertIndexIsValid(startIndex);
    _forEach(callback, startIndex);
    profiler.end();
  };

  bool findIndex(ItemIndexCallback<T, bool> filterFn, size_t &index) {
    profiler.start();
    bool found = _findIndex(filterFn, index);
    profiler.end();
    return found;
  };

  virtual Iterator begin() = 0;
  virtual Iterator end() = 0;

  Profiler *getProfiler() { return &profiler; }
};

template <typename T> void defaultItemRelease(T &item) {
  delete item;
  item = nullptr;
}
