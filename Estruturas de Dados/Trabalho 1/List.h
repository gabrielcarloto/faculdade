#pragma once
#include "BaseList.h"
#include "BasicLinkedList.h"
#include "utils.h"
#include <cstdint>
#include <iostream>

template <class Derived, class Iterator> class TestBaseListDerivedClass;

template <typename T> class ListIterator {
  using ValueType = T;
  using PointerType = T *;
  using ReferenceType = T &;
  using Node = typename BasicLinkedList<T>::Node;

  Node *pointer;

public:
  ListIterator(Node *ptr) : pointer(ptr) {}
  ~ListIterator() { pointer = nullptr; }

  ListIterator &operator++() {
    pointer = pointer->next;
    return *this;
  }

  ListIterator operator++(int) {
    ListIterator it = *this;
    ++(*this);
    return it;
  }

  ListIterator &operator--() {
    pointer = pointer->prev;
    return *this;
  }

  ListIterator operator--(int) {
    ListIterator it = *this;
    --(*this);
    return it;
  }

  ReferenceType operator[](size_t index) {
    size_t i = 0;

    while (i != index) {
      pointer = pointer->next;
      i++;
    }

    return pointer->data;
  }

  PointerType operator->() { return &pointer->data; }

  ReferenceType operator*() { return pointer->data; }

  bool operator==(const ListIterator &other) const {
    return pointer == other.pointer;
  }

  bool operator!=(const ListIterator &other) const { return !(*this == other); }
};

template <typename T>
class List : public BaseList<T, List<T>, ListIterator<T>> {
private:
  using Node = typename BasicLinkedList<T>::Node;

public:
private:
  BasicLinkedList<T> nodes;

  friend class TestBaseListDerivedClass<List<T>, ListIterator<T>>;

  T &_at(intmax_t index) override {
    return nodes.gotoIndex(this->intmax_t_to_size_t(index))->data;
  };

  void _push(const T &item) override {
    nodes.push()->data = item;
    this->length++;
  };

  void _push(const T &&item) override {
    nodes.push()->data = std::move(item);
    this->length++;
  };

  void _remove(size_t index) override {
    nodes.remove(index,
                 [=](Node *node) { this->callReleaseCallback(node->data); });

    this->length--;
  };

  void _insert(const T &item, size_t index = 0) override {
    nodes.insert(index)->data = item;
    this->length++;
  };

  void _insert(const T &&item, size_t index = 0) override {
    nodes.insert(index)->data = std::move(item);
    this->length++;
  };

  void _replace(T item, size_t index = 0) override {
    Node *node = nodes.gotoIndex(index);
    this->callReleaseCallback(node->data);

    node->data = item;
  };

  void _forEach(ItemIndexCallback<T> callback, size_t startIndex = 0) override {
    Node *node = nodes.gotoIndex(startIndex);
    size_t i = startIndex;

    while (node != NULL) {
      callback(node->data, i);
      node = node->next;
      i++;
    }
  };

  bool _findIndex(ItemIndexCallback<T, bool> filterFn, size_t &index) override {
    size_t i = 0;
    Node *node = nodes.first();
    bool found = filterFn(node->data, i);
    this->profiler.addComparison(2);

    while (node->next != NULL && !found) {
      node = node->next;
      i++;

      found = filterFn(node->data, i);
      this->profiler.addComparison();
    }

    if (found)
      index = i;

    return found;
  };

  bool _find(ItemIndexCallback<T, bool> filterFn, T &item) override {
    size_t i = 0;
    Node *node = nodes.first();
    bool found = filterFn(node->data, i);
    this->profiler.addComparison(2);

    while (node->next != NULL && !found) {
      node = node->next;
      i++;

      found = filterFn(node->data, i);
      this->profiler.addComparison();
    }

    found = filterFn(node->data, i);
    if (found)
      item = node->data;

    return found;
  };

  List<T> _filter(ItemIndexCallback<T, bool> filterFn) override {
    List<T> list;

    this->forEach([&](auto item, auto i) {
      bool found = filterFn(item, i);
      this->profiler.addComparison(2);

      if (found)
        list.push(item);
    });

    return list;
  };

  BasicLinkedList<T> &getBasicLinkedList() { return nodes; }

public:
  List(const T &array, const size_t length)
      : BaseList<T, List<T>, ListIterator<T>>(length), nodes(&this->profiler) {
    for (const T item : array) {
      _push(item);
    }
  };

  List(const size_t length = 0)
      : BaseList<T, List<T>, ListIterator<T>>(length), nodes(&this->profiler){};

  ~List() {
    Node *node = nodes.first(), *aux = node != nullptr ? node->next : nullptr;

    while (node != nullptr) {
      this->callReleaseCallback(node->data);
      delete node;

      node = aux;
      if (node != NULL)
        aux = node->next;
    }
  };

  T &operator[](size_t index) override { return nodes.gotoIndex(index)->data; };

  ListIterator<T> begin() override { return ListIterator<T>(nodes.first()); }

  ListIterator<T> end() override { return ListIterator<T>(nullptr); }
};
