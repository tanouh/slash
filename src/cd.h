#ifndef PROJET_SY5_CD_H
#define PROJET_SY5_CD_H

#include <string.h>

int cd_slash(int argc, char **argv);

int cd(char* path);

char *clean (char *path);

char **cut(char *path, size_t*);
#endif //PROJET_SY5_CD_H
