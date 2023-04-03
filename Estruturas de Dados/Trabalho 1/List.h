#pragma once
#include "BaseList.h"
#include "utils.h"
#include <iostream>
#include <stdint.h>

template <class Derived> class TestBaseListDerivedClass;

template <typename T> struct NodeStruct {
  T data;
  NodeStruct<T> *next = NULL;
  NodeStruct<T> *prev = NULL;
};

template <typename T> class List : public BaseList<T, List<T>> {
  using Node = NodeStruct<T>;

  Node *firstNode = NULL;
  Node *lastNode = NULL;

  Node *lastChosenNode = NULL;
  size_t lastChosenNodeIndex = 0;

  Node *gotoIndex(size_t index) {
    this->assertIndexIsValid(index);

    if (index == 0)
      return firstNode;
    else if (index == this->length - 1)
      return lastNode;

    this->profiler.addComparison(2);

    compareIndices(index);

    while (lastChosenNodeIndex < index) {
      lastChosenNode = lastChosenNode->next;
      lastChosenNodeIndex++;
    }

    while (lastChosenNodeIndex > index) {
      lastChosenNode = lastChosenNode->prev;
      lastChosenNodeIndex--;
    }

    return lastChosenNode;
  }

  void compareIndices(size_t index) {
    size_t distanceFirst = index;
    size_t distanceLast = this->length - 1 - index;
    size_t distanceCurrent =
        absolute(static_cast<intmax_t>(index) -
                 static_cast<intmax_t>(lastChosenNodeIndex));

    if (distanceFirst < distanceCurrent) {
      this->profiler.addComparison();

      lastChosenNode = firstNode;
      lastChosenNodeIndex = 0;
    } else if (distanceLast < distanceFirst && distanceLast < distanceCurrent) {
      this->profiler.addComparison();

      lastChosenNode = lastNode;
      lastChosenNodeIndex = this->length - 1;
    }
  }

  friend class TestBaseListDerivedClass<List<T>>;

public:
  List(const T &array, const size_t length) : BaseList<T, List<T>>(length) {
    for (const T item : array) {
      this->push(item);
    }
  };

  List(const size_t length = 0) : BaseList<T, List<T>>(length){};

  ~List() {
    Node *node = firstNode, *aux = node != NULL ? node->next : NULL;

    while (node != NULL) {
      this->callReleaseCallback(node->data);
      delete node;

      node = aux;
      if (node != NULL)
        aux = node->next;
    }
  };

  T &operator[](size_t index) override { return gotoIndex(index)->data; };

  T &_at(intmax_t index) override {
    return gotoIndex(index >= 0 ? index : this->length + index)->data;
  };

  void _push(const T &item) override {
    Node *node = new Node;

    if (firstNode == NULL) {
      this->profiler.addComparison();
      firstNode = node;
      lastChosenNode = node;
    } else {
      this->profiler.addComparison();
      lastNode->next = node;
    }

    node->prev = lastNode;
    lastNode = node;

    node->data = item;
    this->length++;
  };

  void _remove(size_t index) override {
    Node *node = gotoIndex(index), *prevNode = node->prev,
         *nextNode = node->next;

    this->callReleaseCallback(node->data);

    if (prevNode != NULL) {
      this->profiler.addComparison();
      prevNode->next = nextNode;
    }

    if (nextNode != NULL) {
      this->profiler.addComparison();
      nextNode->prev = prevNode;
    }

    if (index == 0) {
      this->profiler.addComparison();
      firstNode = nextNode;
      lastChosenNode = firstNode;
      lastChosenNodeIndex = 0;
    } else if (index == this->length - 1) {
      this->profiler.addComparison();
      lastNode = prevNode;
      lastChosenNode = lastNode;
      lastChosenNodeIndex = this->length - 2;
    } else {
      this->profiler.addComparison();
      lastChosenNode = nextNode;
      lastChosenNodeIndex = index;
    }

    delete node;
    this->length--;
  };

  void _insert(T item, size_t index = 0) override {
    Node *node = gotoIndex(index), *prevNode = node->prev;
    Node *newNode = new Node;

    newNode->data = item;
    newNode->next = node;
    newNode->prev = prevNode;
    node->prev = newNode;

    if (prevNode != NULL) {
      this->profiler.addComparison();
      prevNode->next = newNode;
    }

    if (index == 0)
      firstNode = newNode;

    this->profiler.addMove();
    this->length++;
  };

  void _replace(T item, size_t index = 0) override {
    Node *node = gotoIndex(index);

    this->callReleaseCallback(node->data);
    node->data = item;
  };

  void _forEach(ItemIndexCallback<T> callback, size_t startIndex = 0) override {
    Node *node = gotoIndex(startIndex);
    size_t i = startIndex;

    while (node != NULL) {
      callback(node->data, i);
      node = node->next;
      i++;
    }
  };

  bool _findIndex(ItemIndexCallback<T, bool> filterFn, size_t &index) override {
    size_t i = 0;
    Node *node = firstNode;
    bool found = filterFn(node->data, i);

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
    Node *node = firstNode;
    bool found = filterFn(node->data, i);
    this->profiler.addComparison();

    while (node->next != NULL && !found) {
      node = node->next;
      i++;

      found = filterFn(node->data, i);
      this->profiler.addComparison();
    }

    if (found)
      item = node->data;

    return found;
  };

  List<T> _filter(ItemIndexCallback<T, bool> filterFn) override {
    List<T> list;

    this->forEach([&](auto item, auto i) {
      bool found = filterFn(item, i);
      this->profiler.addComparison();

      if (found)
        list.push(item);
    });

    return list;
  };
};
