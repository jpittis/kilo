#include "trie.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kilo.h"

struct trie *newTrie() {
  struct trie *t = (struct trie*)malloc(sizeof(struct trie));
  t->value = NULL;
  t->node = 0;
  memset(t->next, 0, sizeof(struct trie*) * CHAR_MAX);
  return t;
}

void trieAddKeyValue(struct trie *t, char *key, void *value) {
  struct trie *n = t->next[(int)*key];
  if (n == NULL) {
    n = newTrie();
    n->node = *key;
    t->next[(int)*key] = n;
  }

  // set the value if this is the last byte of the key
  if (*(key+1) == '\0') {
    n->value = value;
  } else {
    trieAddKeyValue(n, key+1, value);
  }
}

struct trie *triePartialLookup(struct trie *t, char *key) {
  struct trie *next;
  if (*key == '\0') {
    return t;
  } else if ((next = t->next[(int)*key])) {
    return triePartialLookup(next, key+1);
  } else {
    return NULL;
  }
}

int trieAccumulateValues(struct trie *t, void **out, int outSize) {
  int off = 0;

  ptrVector *stack = &(ptrVector){0, 0, 0};
  ptrVectorInit(stack);
  ptrVectorPushBack(stack, t);

  while (stack->idx) {
    struct trie *elem = (struct trie *)ptrVectorPopBack(stack);
    for (int i = 0; i < CHAR_MAX; ++i) {
      if (elem->next[i]) {
        out[off++] = elem->next[i];
        if (off == outSize)
          return -1;
        ptrVectorPushBack(stack, elem->next[i]);
      }
    }
  }

  ptrVectorDestroy(stack);
  return off;
}

void *trieLookup(struct trie *t, char *key) {
  struct trie *node = triePartialLookup(t, key);
  if (node)
    return node->value;
  return NULL;
}

char *trieLookupPartialText(struct trie *begin, char *partialName) {
  struct trie *t = triePartialLookup(begin, partialName);
  if (t == NULL) {
    return NULL;
  }

  int i, found;

  int bufsz = 8;
  int len = 0;
  char *buf = malloc(bufsz);

  for (;;) {
    if (t->value != NULL) {
      goto done;
    }
    found = -1;
    for (i = 0; i < CHAR_MAX; i++) {
      if (t->next[i] != NULL) {
        if (found != -1) {
          // There are multiple possible branches in the trie so return the
          // current location.
          goto done;
        }
        found = i;
      }
    }
    t = t->next[found];
    if (len + 1 >= bufsz) { // leave room for a null byte
      buf = realloc(buf, bufsz *= 2);
    }
    buf[len++] = found;
  }

 done:
  buf[len] = '\0';
  return buf;
}
