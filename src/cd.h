#ifndef PROJET_SY5_CD_H
#define PROJET_SY5_CD_H

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

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
int exec_cd(int fdin, int fdout, int fderr, int argc, char **argv);

int cd(char* path, int physical);

char *catPath(char *path, char *realpath);

char *clean (char *path, char *realpath);

char **cut(char *path, size_t path_size, size_t *size);



#endif //PROJET_SY5_CD_H
