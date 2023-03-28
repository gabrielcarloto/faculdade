#pragma once
#include "BaseList.h"
#include <iostream>

template <typename T> class List : public BaseList<T> {
  struct Node {
    T data;
    Node *next = NULL;
  };

  size_t currentIndex = 0;
  Node *firstNode = NULL;
  Node *lastNode = NULL;

public:
  List(const T &array, const size_t length);
  List(const size_t length = 0);
  ~List();

  T &operator[](size_t index) override;

  T &at(intmax_t index) override;
  void push(const T &item) override;
  void remove(size_t index) override;
  void insert(T item, size_t index = 0) override;
  void replace(T item, size_t index = 0) override;
  void forEach(ItemIndexCallback<T> callback, size_t startIndex = 0) override;
  bool findIndex(ItemIndexCallback<T, bool> filterFn, size_t &index) override;
  bool find(ItemIndexCallback<T, bool> filterFn, T &item) override;
  List<T> &filter(ItemIndexCallback<T, bool> filterFn) override;
};

template <typename T> List<T>::List(const T &array, const size_t length) {
  this->length = length;

  for (const T item : array) {
    push(item);
  }
}

template <typename T> List<T>::List(const size_t length) {
  this->length = length;
}

template <typename T> void List<T>::push(const T &item) {
  Node *node = new Node;

  if (firstNode == NULL)
    firstNode = node;
  else
    lastNode->next = node;

  lastNode = node;

  node->data = item;
}
