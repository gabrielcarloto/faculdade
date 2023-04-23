#pragma once
#include "BaseList.h"
#include "utils.h"
#include <cstdint>
#include <iostream>

template <class Derived> class TestBaseListDerivedClass;

template <typename T> struct NodeStruct {
  NodeStruct<T> *next = nullptr;
  NodeStruct<T> *prev = nullptr;
  T data;
};

template <typename T> class List : public BaseList<T, List<T>> {
  using Node = NodeStruct<T>;

  class NodesManager {
    Node *firstNode = nullptr;
    Node *lastNode = nullptr;
    size_t length = 0;

    Node *lastChosenNode = nullptr;
    size_t lastChosenNodeIndex = 0;

    Profiler *profiler;

    void assertIndexIsValid(size_t index) {
      profiler->addComparison();
      if (index < length)
        return;

      char message[100];
      snprintf(message, 100, "Index out of range: %lld", index);

      throw std::out_of_range(message);
    };

  public:
    NodesManager(Profiler *prof) : profiler(prof) {}

    Node *first() const { return firstNode; }
    Node *last() const { return lastNode; }
    [[nodiscard]] size_t size() const { return length; }

    Node *gotoIndex(size_t index) {
      assertIndexIsValid(index); // TODO: remove this function from this class
      profiler->addComparison(2);

      if (index == 0)
        return firstNode;
      else if (index == length - 1)
        return lastNode;

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
      size_t distanceLast = length - 1 - index;
      size_t distanceCurrent =
          absolute(static_cast<intmax_t>(index) -
                   static_cast<intmax_t>(lastChosenNodeIndex));

      profiler->addComparison(2);

      if (distanceFirst < distanceCurrent) {
        lastChosenNode = firstNode;
        lastChosenNodeIndex = 0;
      } else if (distanceLast < distanceFirst &&
                 distanceLast < distanceCurrent) {
        lastChosenNode = lastNode;
        lastChosenNodeIndex = length - 1;
      }
    }

    Node *push() {
      Node *node = new Node;

      profiler->addComparison(2);
      if (firstNode == NULL) {
        firstNode = node;
        lastChosenNode = node;
      } else {
        lastNode->next = node;
      }

      node->prev = lastNode;
      lastNode = node;

      length++;
      return node;
    }

    void remove(size_t index,
                std::function<void(Node *)> beforeDeleteCallback) {
      Node *node = gotoIndex(index), *prevNode = node->prev,
           *nextNode = node->next;

      beforeDeleteCallback(node);
      profiler->addComparison(5);
      profiler->addMove();

      if (prevNode != NULL) {
        prevNode->next = nextNode;
      }

      if (nextNode != NULL) {
        nextNode->prev = prevNode;
      }

      if (index == 0) {
        firstNode = nextNode;
        lastChosenNode = firstNode;
        lastChosenNodeIndex = 0;
      } else if (index == length - 1) {
        lastNode = prevNode;
        lastChosenNode = lastNode;
        lastChosenNodeIndex = length - 2;
      } else {
        lastChosenNode = nextNode;
        lastChosenNodeIndex = index;
      }

      delete node;
    }

    Node *insert(size_t index) {
      Node *node = gotoIndex(index), *prevNode = node->prev;
      Node *newNode = new Node;

      profiler->addComparison(2);
      profiler->addMove();

      newNode->next = node;
      newNode->prev = prevNode;
      node->prev = newNode;

      if (prevNode != NULL) {
        prevNode->next = newNode;
      }

      if (index == 0) {
        firstNode = newNode;
        lastChosenNode = firstNode;
      }

      return newNode;
    }
  };

  NodesManager nodes;

  friend class TestBaseListDerivedClass<List<T>>;

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

  void _insert(T item, size_t index = 0) override {
    nodes.insert(index)->data = item;
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

  NodesManager &getManager() { return nodes; }

public:
  List(const T &array, const size_t length)
      : BaseList<T, List<T>>(length), nodes(&this->profiler) {
    for (const T item : array) {
      _push(item);
    }
  };

  List(const size_t length = 0)
      : BaseList<T, List<T>>(length), nodes(&this->profiler){};

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
};
