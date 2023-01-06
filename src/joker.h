#ifndef PROJET_SY5_JOKER_H
#define PROJET_SY5_JOKER_H
#include <dirent.h>

int getExtremity(char **basePath, char **followPath, token *current);

int verifFile(char *basePath, char *followPath, char *fileName, token *current);

int openFile(char *path, DIR *dir, int depth);

int expand_path(char **argv, struct tokenList **tokList, int posArg, int *nbArg, enum tokenType type);

int expand_double(char **argv, struct tokenList **tokList, int posArg, int *nbArg, enum tokenType type);

#endif //PROJET_SY5_JOKER_H
