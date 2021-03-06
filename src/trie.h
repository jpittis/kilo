#include <limits.h>

#ifndef KILO_TRIE_H
#define KILO_TRIE_H

struct trie {
  void *value; // a node is empty when value is NULL
  char node;
  struct trie *next[CHAR_MAX];
};

struct trie *newTrie();
void destroyTrie(struct trie *t, _Bool destroyValue);
void trieAddKeyValue(struct trie *t, char *key, void *value);
void *trieLookup(struct trie *t, char *key);
struct trie *triePartialLookup(struct trie *t, char *key);
char *trieLookupPartialText(struct trie *t, char *key);
int trieAccumulateValues(struct trie *t, void **out, int outSize);

#endif
