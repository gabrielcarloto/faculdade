#pragma once
#include "BaseList.h"
#include "utils.h"
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <utility>

template <typename T> class Vector : public BaseList<T, Vector<T>> {
  size_t capacity;
  T *data;

  void grow();
  void resizeIfNeeded();
  void commonConstructor();
  void resize(size_t newSize);
  double getGrowthFactor(size_t size);
  void shrink();

public:
  Vector(const T &array, const size_t length);
  Vector(const size_t length = 0);
  ~Vector();

  T &operator[](size_t index) override;

  size_t getCapacity();
  void reserve(size_t capacity);
  void shrinkToFit();

  T *getArray();
  T &_at(intmax_t index) override;
  void _push(const T &item) override;
  void _remove(size_t index) override;
  void _insert(T item, size_t index = 0) override;
  void _replace(T item, size_t index = 0) override;
  void _forEach(ItemIndexCallback<T> callback, size_t startIndex = 0) override;
  bool _findIndex(ItemIndexCallback<T, bool> filterFn, size_t &index) override;
  bool _find(ItemIndexCallback<T, bool> filterFn, T &item) override;
  Vector<T> _filter(ItemIndexCallback<T, bool> filterFn) override;
};

template <typename T>
Vector<T>::Vector(const T &array, const size_t length)
    : BaseList<T, Vector<T>>(length) {
  commonConstructor();
  memcpy(data, array, length * sizeof(T)); // TODO: switch to for loop
}

template <typename T>
Vector<T>::Vector(const size_t length) : BaseList<T, Vector<T>>(length) {
  commonConstructor();
}

template <typename T> Vector<T>::~Vector() {
  if (this->checkReleaseCallback())
    for (size_t i = 0; i < this->length; i++)
      this->rawCallReleaseCallback(data[i]);

  delete[] data;
}

template <typename T> void Vector<T>::commonConstructor() {
  this->capacity = this->length + 1;
  data = DBG_NEW T[this->capacity];
}

template <typename T> void Vector<T>::reserve(size_t capacity) {
  this->profiler.addComparison();

  if (this->capacity > capacity)
    return;

  resize(capacity);
}

template <typename T> void Vector<T>::resize(size_t newSize) {
  capacity = newSize;
  T *tempArray = DBG_NEW T[newSize];

  for (size_t i = 0; i < this->length; i++) {
    tempArray[i] = data[i];
    this->profiler.addMove();
  }

  delete[] data;
  data = tempArray;
}

template <typename T> void Vector<T>::grow() {
  resize(capacity * getGrowthFactor(capacity));
}

template <typename T> void Vector<T>::shrink() {
  resize(this->length * getGrowthFactor(this->length));
}

template <typename T> void Vector<T>::shrinkToFit() { resize(this->length); }

template <typename T> void Vector<T>::resizeIfNeeded() {
  this->profiler.addComparison(2);

  if (this->length >= capacity)
    grow();

  // else if (this->length > 0 &&
  //          this->length * (getGrowthFactor(this->length) + 0.7) <
  //              capacity) // TODO: remove hard-coded number
  //
  //   shrink();
}

template <typename T>
void Vector<T>::_forEach(
    const std::function<void(T &item, const size_t index)> &callback,
    size_t startIndex) {
  for (size_t i = startIndex; i < this->length; i++) {
    callback(data[i], i);
  }
}

template <typename T> void Vector<T>::_insert(T item, size_t index) {
  T lastItem = data[index];
  data[index] = item;

  this->length++;
  resizeIfNeeded();

  this->forEach(
      [&](auto curr, auto i) {
        data[i] = lastItem;
        lastItem = curr;
        this->profiler.addMove();
      },
      index + 1);
}

template <typename T> void Vector<T>::_replace(T item, size_t index) {
  this->callReleaseCallback(data[index]);
  data[index] = item;
}

template <typename T> void Vector<T>::_remove(size_t index) {
  this->callReleaseCallback(data[index]);

  this->forEach(
      [&](auto, auto i) {
        data[i] = data[i + 1];
        this->profiler.addMove();
      },
      index);

  this->length--;
  resizeIfNeeded();
}

template <typename T> void Vector<T>::_push(const T &item) {
  resizeIfNeeded();

  data[this->length] = item;
  this->length++;
}

template <typename T>
Vector<T> Vector<T>::_filter(ItemIndexCallback<T, bool> filterFunction) {
  Vector<T> items;

  for (size_t i = 0; i < this->length; i++) {
    bool shouldBeIncluded = filterFunction(data[i], i);
    this->profiler.addComparison();

    if (shouldBeIncluded)
      items.push(data[i]);
  }

  return items;
}

template <typename T>
bool Vector<T>::_find(ItemIndexCallback<T, bool> filterFunction, T &item) {
  for (size_t i = 0; i < this->length; i++) {
    bool foundItem = filterFunction(data[i], i);
    this->profiler.addComparison();

    if (foundItem) {
      this->profiler.addComparison();

      item = data[i];
      return true;
    }
  }

  return false;
}

template <typename T>
bool Vector<T>::_findIndex(ItemIndexCallback<T, bool> filterFunction,
                           size_t &index) {
  for (size_t i = 0; i < this->length; i++) {
    bool foundItem = filterFunction(data[i], i);
    this->profiler.addComparison();

    if (foundItem) {
      this->profiler.addComparison();

      index = i;
      return true;
    }
  }

  return false;
}

template <typename T> T *Vector<T>::getArray() { return data; }

template <typename T> T &Vector<T>::_at(intmax_t index) {
  this->profiler.addComparison();
  return data[this->intmax_t_to_size_t(index)];
}

template <typename T> T &Vector<T>::operator[](size_t index) {
  return data[index];
}

template <typename T> size_t Vector<T>::getCapacity() { return capacity; }

template <typename T> double Vector<T>::getGrowthFactor(size_t size) {
  enum Thresholds { SMALL = 1000, MEDIUM = 10000, LARGE = 50000 };
  const double smallFactor = 2, mediumFactor = 1.4, largeFactor = 1.2,
               defaultFactor = 1.1;

  this->profiler.addComparison(3);

  if (size <= SMALL)
    return smallFactor;
  if (size <= MEDIUM)
    return mediumFactor;
  if (size <= LARGE)
    return largeFactor;

  // TODO: possível perda de dados a oconverter double para size_t
  return defaultFactor;
}
