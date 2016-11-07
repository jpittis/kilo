#ifndef KILO_COLON_H
#define KILO_COLON_H

#define HISTORY_SIZE 5

typedef void colonFunction();
typedef _Bool unaryColonFunction(char *);

int handleColonFunction(char *name, char *arg);
void registerColonFunction(char *name, colonFunction *func);
void registerColonFunctionWithArg(char *name, unaryColonFunction *func);
char *lookupPartialColonFunction(char *partialName);
char *getHistoryAt(int i);

#endif
