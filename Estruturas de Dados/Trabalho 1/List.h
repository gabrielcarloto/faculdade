#pragma once
#include "BaseList.h"
#include "utils.h"
#include <iostream>
#include <stdint.h>

template <typename T> class List : public BaseList<T, List<T>> {
  struct Node {
    T data;
    Node *next = NULL;
    Node *prev = NULL;
  };

  Node *firstNode = NULL;
  Node *lastNode = NULL;

  Node *lastChosenNode = NULL;
  size_t lastChosenNodeIndex = 0;

  Node *gotoIndex(size_t index) {
    this->assertIndexIsValid(index);
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
      lastChosenNode = firstNode;
      lastChosenNodeIndex = 0;
    } else if (distanceLast < distanceFirst && distanceLast < distanceCurrent) {
      lastChosenNode = lastNode;
      lastChosenNodeIndex = this->length - 1;
    }
  }

public:
  List(const T &array, const size_t length) : BaseList<T, List<T>>(length) {
    for (const T item : array) {
      push(item);
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
      firstNode = node;
      lastChosenNode = node;
    } else {
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
    prevNode->next = nextNode;
    nextNode->prev = prevNode;
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

    if (prevNode != NULL)
      prevNode->next = newNode;

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

    while (node != NULL && !found) {
      node = node->next;
      i++;

      found = filterFn(node->data, i);
    }

    if (found)
      index = i;

    return found;
  };

  bool _find(ItemIndexCallback<T, bool> filterFn, T &item) override {
    size_t i = 0;
    Node *node = firstNode;
    bool found = filterFn(node->data, i);

    while (node != NULL && !found) {
      node = node->next;
      i++;

      found = filterFn(node->data, i);
    }

    if (found)
      item = node->data;

    return found;
  };

  List<T> _filter(ItemIndexCallback<T, bool> filterFn) override {
    List<T> list;

    this->forEach([&](auto item, auto i) {
      bool found = filterFn(item, i);

      if (found)
        list.push(item);
    });

    return list;
  };
};
