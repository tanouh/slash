#ifndef PROJET_SY5_JOKER_H
#define PROJET_SY5_JOKER_H
#include <dirent.h>

int getExtremity(char **basePath, char **followPath, char **argv, int posArg);

int filter (const struct dirent *);

void expand_cmd(char *cmd);

int expand_path(char **argv, token **first, token **last, int posArg, int *nbArg);

#endif //PROJET_SY5_JOKER_H
