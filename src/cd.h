#ifndef PROJET_SY5_CD_H
#define PROJET_SY5_CD_H

/**
 * Changes the current directory as ref if valid, as the last work directory if '-', or $HOME by default
 * @param argc the number of arguments
 * @param argv the arguments of the command
 * Acceptef format [-L | -P] [ref | -]
 * -P : interprets ref with in regards to the physical structure of the tree structure
 * -L : default if it makes sense or like P otherwise
 * (does not include the name of the command)
 * @return 0 if succes, 1 if error
 */
int exec_cd(int argc, char **argv);

int cd(char* path, int physical);

char *clean (char *path, char *realpath);

char **cut(char *path, size_t*);

char *removeFirstSlash(char *path);

char *addLastSlash(char *path);

#endif //PROJET_SY5_CD_H
