#include "colon.h"
#include "kilo.h"
#include "trie.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static char* history[HISTORY_SIZE];

static struct trie colonFunctions;

char *lookupPartialColonFunction(char *partialName) {
  struct trie *t = triePartialLookup(&colonFunctions, partialName);
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

void *lookupColonFunction(char *name) {
  return trieLookup(&colonFunctions, name);
}

void addToHistory(char *str) {
  int len = strlen(str);
  char *added = malloc(len + 1);
  strcpy(added, str);

  free(history[HISTORY_SIZE-1]);

  // Shift all the entries up one.
  int i;
  for (i = HISTORY_SIZE-2; i >= 0; i--) {
    history[i+1] = history[i];
  }
  history[0] = added;
}

int handleColonFunction(char *name, char *arg) {
  // A sketchy hack that relies on name and arg being right beside eachother in
  // memory.
  if (arg != NULL) {
    int length = strlen(name);
    name[length] = ' ';
    addToHistory(name);
    name[length] = '\0';
  } else {
    addToHistory(name);
  }

  void *func = lookupColonFunction(name);
  logmsg("retreiving: %s: %p\n", name, func);
  if (((ptrdiff_t)func) & 0x1) {
    ptrdiff_t unmask = (ptrdiff_t)func ^ 0x1;
    ((unaryColonFunction *)(void *)unmask)(arg);
    return 0;
  } else if (func) {
    ((colonFunction *)func)();
    return 0;
  } else {
    return 1;
  }
}

char *getHistoryAt(int i) {
  if (i < 0 || i >= HISTORY_SIZE) {
    return NULL;
  }
  return history[i];
}

void registerColonFunction(char *name, colonFunction *func) {
  logmsg("registering %s: %p\n", name, func);
  trieAddKeyValue(&colonFunctions, name, (void *)func);
}

void registerColonFunctionWithArg(char *name, unaryColonFunction *func) {
  /* Tag pointer as taking an argument */
  ptrdiff_t mask = (ptrdiff_t)func | 0x1;
  logmsg("registering arg func %s: %p\n", name, func);
  trieAddKeyValue(&colonFunctions, name, (void *)mask);
}
