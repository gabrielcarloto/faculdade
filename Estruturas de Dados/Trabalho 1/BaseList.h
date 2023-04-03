#pragma once
#include "Profiler.h"
#include <functional>
#include <iostream>
#include <stdint.h>

template <typename TItem, typename TReturn = void>
using ItemIndexCallback =
    const std::function<TReturn(TItem &item, const size_t index)> &;

template <typename T> void defaultItemRelease(T &item);

template <typename T, typename Derived> class BaseList {
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

  void (*itemReleaseCallback)(T &) = NULL;

protected:
  size_t length;
  Profiler profiler;

  bool checkReleaseCallback() { return itemReleaseCallback != NULL; };

  void callReleaseCallback(T &item) {
    profiler.addComparison();
    if (itemReleaseCallback != NULL)
      (*itemReleaseCallback)(item);
  };

  void rawCallReleaseCallback(T &item) { (*itemReleaseCallback)(item); };

  void assertIndexIsValid(size_t index) {
    profiler.addComparison();
    if (index < length)
      return;

    throwOutOfRange(index);
  };

  void assertIndexIsValid(intmax_t index) {
    if (index < static_cast<intmax_t>(length) &&
        static_cast<intmax_t>(length) + index > 0)
      return;

    throwOutOfRange(index);
  };

  virtual T &_at(intmax_t index) = 0;
  virtual void _push(const T &item) = 0;
  virtual void _remove(size_t index) = 0;
  virtual void _insert(T item, size_t index = 0) = 0;
  virtual void _replace(T item, size_t index = 0) = 0;
  // virtual void slice(size_t indexStart, size_t indexEnd = NULL);
  virtual Derived _filter(ItemIndexCallback<T, bool> filterFn) = 0;
  virtual bool _find(ItemIndexCallback<T, bool> filterFn, T &item) = 0;
  virtual void _forEach(ItemIndexCallback<T> callback,
                        size_t startIndex = 0) = 0;

  virtual bool _findIndex(ItemIndexCallback<T, bool> filterFn,
                          size_t &index) = 0;

public:
  BaseList(const size_t length) : profiler() { this->length = length; }
  ~BaseList() = default;

  void registerItemReleaseCallback(void (*fn)(T &)) {
    itemReleaseCallback = fn;
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

  void remove(size_t index) {
    profiler.start();
    assertIndexIsValid(index);
    _remove(index);
    profiler.end();
  };

  void insert(T item, size_t index = 0) {
    profiler.start();

    if (index == length) {
      _push(item);
      return profiler.end();
    } else if (index == length - 1) {
      assertIndexIsValid(index);
      T lastItem = _at(index);
      _replace(item, index);
      _push(lastItem);
      return profiler.end();
    }

    assertIndexIsValid(index);

    _insert(item, index);
    profiler.end();
  };

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

  Profiler *getProfiler() { return &profiler; }
};

template <typename T> void defaultItemRelease(T &item) { delete item; }
