#pragma once
#include "BaseList.h"
#include "utils.h"
#include <iostream>

template <typename T> class List : public BaseList<T> {
  struct Node {
    T data;
    Node *next = NULL;
    Node *prev = NULL;
  };

  Node *firstNode = NULL;
  Node *lastNode = NULL;
  Node *current = NULL;
  size_t currentIndex = 0;

  Node *gotoIndex(size_t index) {
    intmax_t distanceFirst = index, distanceCurrent = abs(index - currentIndex),
             distanceLast = abs(index - this->length - 1);

    if (distanceFirst < distanceCurrent) {
      current = firstNode;
      currentIndex = 0;
    } else if (distanceLast < distanceFirst && distanceLast < distanceCurrent) {
      current = lastNode;
      currentIndex = this->length - 1;
    }

    for (currentIndex; currentIndex < index; currentIndex++) {
      current = current->next;
    }

    for (currentIndex; currentIndex > index; currentIndex--) {
      current = current->prev;
    }

    return current;
  }

public:
  List(const T &array, const size_t length) {
    this->length = length;

    for (const T item : array) {
      push(item);
    }
  };

  List(const size_t length = 0) { this->length = length; };

  ~List();

  T &operator[](size_t index) override;

  T &at(intmax_t index) override;

  void push(const T &item) override {
    Node *node = new Node;

    if (firstNode == NULL) {
      firstNode = node;
      currentIndex = node;
    } else {
      lastNode->next = node;
    }

    node->prev = lastNode;
    lastNode = node;

    node->data = item;
  };

  void remove(size_t index) override;
  void insert(T item, size_t index = 0) override;
  void replace(T item, size_t index = 0) override;
  void forEach(ItemIndexCallback<T> callback, size_t startIndex = 0) override;
  bool findIndex(ItemIndexCallback<T, bool> filterFn, size_t &index) override;
  bool find(ItemIndexCallback<T, bool> filterFn, T &item) override;
  List<T> &filter(ItemIndexCallback<T, bool> filterFn) override;
};
