#pragma once
#include <functional>
#include <iostream>
#include <stdint.h>

template <typename TItem, typename TReturn = void>
using ItemIndexCallback =
    const std::function<TReturn(TItem &item, const size_t index)> &;

template <typename T> void defaultItemRelease(T &item);

template <typename T> class BaseList {
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

  bool checkReleaseCallback() { return itemReleaseCallback != NULL; };

  void callReleaseCallback(T &item) {
    if (itemReleaseCallback != NULL)
      (*itemReleaseCallback)(item);
  };

  void rawCallReleaseCallback(T &item) { (*itemReleaseCallback)(item); };
  void assertIndexIsValid(size_t index) {
    if (index <= length)
      return;

    throwOutOfRange(index);
  };

  void assertIndexIsValid(intmax_t index) {
    if (index <= length && index >= -static_cast<intmax_t>(length))
      return;

    throwOutOfRange(index);
  };

  virtual T &_at(intmax_t index) = 0;
  virtual void _push(const T &item) = 0;
  virtual void _remove(size_t index) = 0;
  virtual void _insert(T item, size_t index = 0) = 0;
  virtual void _replace(T item, size_t index = 0) = 0;
  // virtual void slice(size_t indexStart, size_t indexEnd = NULL);
  virtual BaseList<T> &_filter(ItemIndexCallback<T, bool> filterFn) = 0;
  virtual bool _find(ItemIndexCallback<T, bool> filterFn, T &item) = 0;
  virtual void _forEach(ItemIndexCallback<T> callback,
                        size_t startIndex = 0) = 0;

  virtual bool _findIndex(ItemIndexCallback<T, bool> filterFn,
                          size_t &index) = 0;

public:
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
    assertIndexIsValid(index);
    _at(index);
  };

  void push(const T &item) { _push(item); };
  void remove(size_t index) {
    assertIndexIsValid(index);
    _remove(index);
  };

  void insert(T item, size_t index = 0) {
    assertIndexIsValid(index);
    _insert();
  };

  void replace(T item, size_t index = 0) {
    assertIndexIsValid(index);
    _replace();
  };

  BaseList<T> &filter(ItemIndexCallback<T, bool> filterFn) {
    _filter(filterFn);
  };

  bool find(ItemIndexCallback<T, bool> filterFn, T &item) {
    _find(filterFn, item);
  };

  void forEach(ItemIndexCallback<T> callback, size_t startIndex = 0) {
    assertIndexIsValid(startIndex);
    _forEach(callback, startIndex);
  };

  // TODO: use pointer instead
  bool findIndex(ItemIndexCallback<T, bool> filterFn, size_t &index) {
    _findIndex(filterFn, index);
  };
};

template <typename T> void defaultItemRelease(T &item) { delete item; }
