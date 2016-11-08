#include "colon.h"
#include "kilo.h"
#include "trie.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

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

char *parseColonString(char *inp) {
  char *space = strchr(inp, ' ');
  if (space) {
    *space = '\0';
    ++space;
  }
  return space;
}

int colonCompleteCallback(char **strPtr, int bufsz) {
  char *str = *strPtr;
  struct trie *t;
  char *toSearch;

  char *space = parseColonString(str);
  if (space) {
    bool isBuffer = strcmp("b", str) == 0;
    bool isOpen = strcmp("e", str) == 0;
    *(space - 1) = ' ';
    if (isBuffer) {
      t = &openBuffers;
      toSearch = space;
    } else if (isOpen) {
      DIR *dir;
      dir = opendir(".");
      struct dirent *entry;
      if (!dir)
        return bufsz;
      t = newTrie();
      while ((entry = readdir(dir)))
        trieAddKeyValue(t, entry->d_name, (void *)1);
      toSearch = space;
    } else
      return bufsz;
  } else if (!space) {
    t = &colonFunctions;
    toSearch = str;
  } else {
    /* No possible completion. */
    return bufsz;
  }

  char *autocomplete = trieLookupPartialText(t, toSearch);
  if (!autocomplete) {
    return bufsz;
  }

  int autolen = strlen(autocomplete);
  int origlen = strlen(str);
  while (autolen + origlen + 1 > bufsz)
    str = realloc(str, bufsz *= 2);
  strcpy(str + origlen, autocomplete);
  *strPtr = str;

  return bufsz;
}

int handleColonFunction(char *name) {
  addToHistory(name);

  char *space = parseColonString(name);

  void *func = trieLookup(&colonFunctions, name);
  if (((ptrdiff_t)func) & 0x1) {
    ptrdiff_t unmask = (ptrdiff_t)func ^ 0x1;
    ((unaryColonFunction *)(void *)unmask)(space);
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
