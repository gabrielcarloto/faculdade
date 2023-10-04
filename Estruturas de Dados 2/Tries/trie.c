#include "trie.h"

/* */
Trie *create_node(char data) {
  Trie *node = (Trie *)malloc(sizeof(Trie));

  node->data = data;
  node->end = FALSE;

  for (int i = 0; i < size; i++) {
    node->keys[i] = NULL;
  }

  return node;
}

/* */
void free_node(Trie *node) {
  for (int i = 0; i < size; i++) {
    if (node->keys[i] != NULL) {
      free_node(node->keys[i]);
    }
  }

  free(node);
}

/* */
Trie *insert(Trie *root, char *word) {
  if (!root)
    return NULL;

  Trie *aux = root;
  int i = 0;

  for (i = 0; word[i] != '\0'; i++) {
    int nextCharIndex = word[i] - 'a';

    if (!aux->keys[nextCharIndex])
      aux->keys[nextCharIndex] = create_node(word[i]);

    aux = aux->keys[nextCharIndex];
  }

  aux->end = TRUE;

  return root;
}

/* */
int search_auxiliary(Trie *root, char *word) {
  int i = 0;

  if (!root || !word)
    return FALSE;

  for (i = 0; word[i] != '\0'; i++) {
    int nextCharIndex = word[i] - 'a';
    Trie *nextChar = root->keys[nextCharIndex];

    if (nextChar) {
      root = nextChar;
    } else {
      return FALSE;
    }
  }

  return word[i] == '\0' && root->end;
}

void search(Trie *root, char *word) {
  if (search_auxiliary(root, word))
    printf("Word %s found!\n", word);
  else
    printf("Error: word %s not found!\n", word);
}

/* */
void print(Trie *root, int level) {
  if (root != NULL) {
    int i;
    if (root->data == '\0')
      printf("(null)");
    for (i = 0; i < level; i++) {
      printf("| ");
    }
    printf("%c (%d)\n", root->data, root->end);
    for (i = 0; i < size; i++) {
      print(root->keys[i], level + 1);
    }
  }
}

int has_children(Trie *node) {
  for (int i = 0; i < size; i++) {
    if (node->keys[i])
      return TRUE;
  }

  return FALSE;
}

Trie *delete(Trie *root, char *word) {
  if (!root || !word)
    return root;

  if (*word != '\0') {
    int nextCharIndex = word[0] - 'a';
    Trie *nextChar = root->keys[nextCharIndex];

    if (!nextChar)
      return root;

    root->keys[nextCharIndex] = delete (nextChar, word + 1);

  } else {
    root->end = FALSE;
  }

  if (!has_children(root)) {
    free(root);
    root = NULL;
  }

  return root;
}