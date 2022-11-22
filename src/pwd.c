#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "pwd.h"

int parser_pwd(int argc, char *argv[], int fdout) {
        if (argc == 1) {
                return pwdL(fdout);
        }
        //Si on doit faire pwd retourne meme si les argument ne sont pas valide enlever cette ligne et l'autre signalé
        if (argc > 2) {
                write(STDERR_FILENO, "-slash: too many arguments \n", strlen("-slash: too many arguments \n"));
                return 1;
        }

        if ((argv[1]) && (!strcmp((argv[1]), "-L"))) {

                return pwdL(fdout);
        }
        if ((argv[1]) && (!strcmp((argv[1]), "-P"))) {
                return pwdP(fdout);
        }
        //Celle-ci
        write(STDERR_FILENO, "-slash: invalid option \n", strlen("-slash: invalid option \n"));
        return 1;
}

int pwdP(int fdout) {
        char path[256];
        if (getcwd(path, sizeof(path)) == NULL) {
                write(STDERR_FILENO, "Something goes wrong with getcwd\n",
                      strlen("Something goes wrong with getcwd\n"));

                return 1;
        }
        write(fdout, path, strlen(path) + 2);

        return 0;
}

int pwdL(int fdout) {
        char *path = getenv("PWD");
        if (path == NULL) {
                write(STDERR_FILENO, "Something goes wrong with getenv\n",
                      strlen("Something goes wrong with getenv\n"));
                return 1;
        }
        write(fdout, path, strlen(path) + 1);
        return 0;
}