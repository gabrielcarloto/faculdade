#include "Profiler.h"
#include "utils.h"

#define absolute(x) (x > 0 ? x : -x)

template <class T> class BasicLinkedList {
public:
  struct Node {
    Node *next = nullptr;
    Node *prev = nullptr;
    T data;
  };

private:
  Node *firstNode = nullptr;
  Node *lastNode = nullptr;
  size_t length = 0;

  Node *lastChosenNode = nullptr;
  size_t lastChosenNodeIndex = 0;

  Profiler *profiler;

  void compareIndices(size_t index) {
    size_t distanceFirst = index;
    size_t distanceLast = length - 1 - index;
    size_t distanceCurrent = absolute(
        static_cast<intmax_t>(index) -
        static_cast<intmax_t>(
            lastChosenNodeIndex)); // BUG: usar o absolute causa overflow em
                                   // alguns casos, fazendo com que a lista
                                   // demore mais para encontrar o índice. Isso
                                   // deve ser trocado pelo std::abs, no entanto
                                   // isso também leva a problemas para
                                   // encontrar o índice, então deve ser
                                   // investigado.

    profiler->addComparison(2);

    if (distanceFirst < distanceCurrent) {
      lastChosenNode = firstNode;
      lastChosenNodeIndex = 0;
    } else if (distanceLast < distanceFirst && distanceLast < distanceCurrent) {
      lastChosenNode = lastNode;
      lastChosenNodeIndex = length - 1;
    }
  }

public:
  BasicLinkedList(Profiler *prof) : profiler(prof) {}

  Node *first() const { return firstNode; }
  Node *last() const { return lastNode; }
  [[nodiscard]] size_t size() const { return length; }

  Node *gotoIndex(size_t index) {
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

  Node *push() {
    Node *node = new Node;

    profiler->addComparison();
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
              const std::function<void(Node *)> &beforeDeleteCallback) {
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

    length--;
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

    length++;
    return newNode;
  }

  /**
   * @brief Allocates new nodes in the list without adding data. This function
   * can be used to access arbitrary indexes before adding any data, for
   * example, when dealing with a non-in-place sorting algorithm.
   *
   * Note that calling this function with a value of X will create X new nodes,
   * affecting the current size of the list. If you subsequently push an item
   * to the list, it will be added post the last node.
   */
  void allocate(size_t size) {
    if (size < length)
      return;

    for (size_t i = length; i < size; i++)
      push();
  }
};
