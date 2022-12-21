#ifndef PROJET_SY5_JOKER_H
#define PROJET_SY5_JOKER_H
#include <dirent.h>

int getExtremity(char **basePath, char **followPath, char **argv, int posArg);

int filter (const struct dirent *);

int openFile(char *path, DIR *dir, int depth);

void expand_cmd(char *cmd);

int expand_path(char **argv, struct tokenList **tokList, int posArg, int *nbArg, enum tokenType type);

int expand_double(char **argv, struct tokenList **tokList, int posArg, int *nbArg, enum tokenType type);

#endif //PROJET_SY5_JOKER_H
