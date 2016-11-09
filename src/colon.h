#ifndef KILO_COLON_H
#define KILO_COLON_H

#define HISTORY_SIZE 5

extern struct trie colonFunctions;

typedef void colonFunction();
typedef _Bool unaryColonFunction(char *);

int handleColonFunction(char *name);
void registerColonFunction(char *name, colonFunction *func);
void registerColonFunctionWithArg(char *name, unaryColonFunction *func);
char *lookupPartialColonFunction(char *partialName);
char *getHistoryAt(int i);
int colonCompleteCallback(char **str, int buf);

#endif
