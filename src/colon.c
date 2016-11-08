#include "colon.h"
#include "kilo.h"
#include "trie.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static char* history[HISTORY_SIZE];

struct trie colonFunctions;

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

  void *func = trieLookup(&colonFunctions, name);
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
  trieAddKeyValue(&colonFunctions, name, (void *)func);
}

void registerColonFunctionWithArg(char *name, unaryColonFunction *func) {
  /* Tag pointer as taking an argument */
  ptrdiff_t mask = (ptrdiff_t)func | 0x1;
  trieAddKeyValue(&colonFunctions, name, (void *)mask);
}
