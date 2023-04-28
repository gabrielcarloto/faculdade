#pragma once
#include "BaseList.h"
#include "utils.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>

template <typename T> class VectorIterator {
  using ValueType = T;
  using PointerType = T *;
  using ReferenceType = T &;

  PointerType pointer;

public:
  VectorIterator(PointerType ptr) : pointer(ptr) {}
  ~VectorIterator() { pointer = nullptr; }

  VectorIterator &operator++() {
    pointer++;
    return *this;
  }

  VectorIterator operator++(int) {
    VectorIterator it = *this;
    ++(*this);
    return it;
  }

  VectorIterator &operator--() {
    pointer--;
    return *this;
  }

  VectorIterator operator--(int) {
    VectorIterator it = *this;
    --(*this);
    return it;
  }

  ReferenceType operator[](size_t index) { return *(pointer + index); }

  PointerType operator->() { return pointer; }

  ReferenceType operator*() { return *pointer; }

  bool operator==(const VectorIterator &other) const {
    return pointer == other.pointer;
  }

  bool operator!=(const VectorIterator &other) const {
    return !(*this == other);
  }
};

template <typename T>
class Vector : public BaseList<T, Vector<T>, VectorIterator<T>> {

private:
  size_t capacity;
  T *data;

  void grow();
  void shrink();
  void resizeIfNeeded();
  void commonConstructor();
  void resize(size_t newSize);
  double getGrowthFactor(size_t size);

  void copy(T *source, T *dest, size_t sourceLength, size_t destLength) {
    for (size_t i = 0; i < destLength; i++) {
      dest[i] = std::move(source[i]);
      this->profiler.addComparison();
      this->profiler.addMove();
    }

    this->profiler.addComparison();
    if (sourceLength > destLength)
      freeData(destLength);
  }

  void freeData(size_t start = 0, size_t end = 0) {
    this->profiler.addComparison();
    if (this->length > 0 && (start >= this->length || end > this->length))
      throw std::out_of_range("Invalid range in freeData");

    this->profiler.addComparison();
    if (end == 0)
      end = this->length;

    this->profiler.addComparison();
    if (this->checkReleaseCallback())
      for (size_t i = start; i < end; i++) {
        this->rawCallReleaseCallback(data[i]);
        this->profiler.addComparison();
      }
  }

  T &_at(intmax_t index) override;
  void _push(const T &item) override;
  void _push(const T &&item) override;
  void _remove(size_t index) override;
  void _insert(const T &item, size_t index = 0) override;
  void _insert(const T &&item, size_t index = 0) override;
  void _replace(T item, size_t index = 0) override;
  void _forEach(ItemIndexCallback<T> callback, size_t startIndex = 0) override;
  bool _findIndex(ItemIndexCallback<T, bool> filterFn, size_t &index) override;
  bool _find(ItemIndexCallback<T, bool> filterFn, T &item) override;
  Vector<T> _filter(ItemIndexCallback<T, bool> filterFn) override;

public:
  Vector(const T *array, const size_t length);
  Vector(const size_t length = 0);
  ~Vector();

  T &operator[](size_t index) override;

  size_t getCapacity();
  void reserve(size_t capacity);
  void shrinkToFit();

  T *getArray();

  VectorIterator<T> begin() override { return VectorIterator(data); }

  VectorIterator<T> end() override {
    return VectorIterator(data + this->length);
  }
};

template <typename T>
Vector<T>::Vector(const T *array, const size_t length)
    : BaseList<T, Vector<T>, VectorIterator<T>>(length) {
  commonConstructor();
  copy(array, data, length, length);
}

template <typename T>
Vector<T>::Vector(const size_t length)
    : BaseList<T, Vector<T>, VectorIterator<T>>(length) {
  commonConstructor();
}

template <typename T> Vector<T>::~Vector() {
  freeData();
  delete[] data;
}

template <typename T> void Vector<T>::commonConstructor() {
  this->capacity = this->length + 1;
  data = new T[this->capacity];
}

template <typename T> void Vector<T>::reserve(size_t capacity) {
  this->profiler.addComparison();
  if (this->capacity > capacity)
    return;

  resize(capacity);
}

template <typename T> void Vector<T>::resize(size_t newSize) {
  T *tempArray = new T[newSize];
  int isCapacityLessThanLength = capacity < this->length;
  size_t lengthToCopy = this->length - isCapacityLessThanLength;

  copy(data, tempArray, lengthToCopy,
       this->length > newSize ? newSize - isCapacityLessThanLength
                              : lengthToCopy);

  if (this->length > newSize)
    this->length = newSize;

  delete[] data;
  data = tempArray;
  capacity = newSize;
}

template <typename T> void Vector<T>::grow() {
  resize(capacity * getGrowthFactor(capacity));
}

template <typename T> void Vector<T>::shrink() {
  resize(this->length * getGrowthFactor(this->length));
}

template <typename T> void Vector<T>::shrinkToFit() { resize(this->length); }

template <typename T> void Vector<T>::resizeIfNeeded() {
  this->profiler.addComparison();

  if (this->length >= capacity)
    grow();
}

template <typename T>
void Vector<T>::_forEach(
    const std::function<void(T &item, const size_t index)> &callback,
    size_t startIndex) {
  for (size_t i = startIndex; i < this->length; i++) {
    callback(data[i], i);
  }
}

template <typename T> void Vector<T>::_insert(const T &item, size_t index) {
  T lastItem = std::move(data[index]);
  data[index] = std::move(item);

  this->length++;
  resizeIfNeeded();

  this->forEach(
      [&](auto curr, auto i) {
        data[i] = std::move(lastItem);
        lastItem = std::move(curr);
        this->profiler.addMove();
      },
      index + 1);
}

template <typename T> void Vector<T>::_insert(const T &&item, size_t index) {
  T lastItem = std::move(data[index]);
  data[index] = std::move(item);

  this->length++;
  resizeIfNeeded();

  this->forEach(
      [&](auto curr, auto i) {
        data[i] = std::move(lastItem);
        lastItem = std::move(curr);
        this->profiler.addMove();
      },
      index + 1);
}

template <typename T> void Vector<T>::_replace(T item, size_t index) {
  this->callReleaseCallback(data[index]);
  data[index] = std::move(item);
}

template <typename T> void Vector<T>::_remove(size_t index) {
  this->callReleaseCallback(data[index]);

  for (size_t i = index; i < this->length - 1; i++) {
    data[i] = std::move(data[i + 1]);
    this->profiler.addMove();
  }

  this->length--;
  resizeIfNeeded();
}

template <typename T> void Vector<T>::_push(const T &item) {
  resizeIfNeeded();

  data[this->length] = item;
  this->length++;
}

template <typename T> void Vector<T>::_push(const T &&item) {
  resizeIfNeeded();

  data[this->length] = std::move(item);
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
