#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "pwd.h"

int exec_pwd(int argc, char *argv[]) {
        if (argc == 0) {
                return pwdL(STDOUT_FILENO);
        }
        //Si on doit faire pwd retourne meme si les argument ne sont pas valide enlever cette ligne et l'autre signalé
        if (argc > 1) {
                write(STDERR_FILENO, "-slash: too many arguments \n", strlen("-slash: too many arguments \n"));
                return 1;
        }

        if ((argc == 1) && (!strcmp((argv[0]), "-L"))) {
                return pwdL(STDOUT_FILENO);
        }
        if ((argc == 1) && (!strcmp((argv[0]), "-P"))) {
                return pwdP(STDOUT_FILENO);
        }
        //Celle-ci
        write(STDERR_FILENO, "-slash: invalid option \n", strlen("-slash: invalid option \n"));
        return 0;
}

int pwdP(int fdout) {
        char path[256];
        if (getcwd(path, sizeof(path)) == NULL) {
                write(STDERR_FILENO, "Something goes wrong with getcwd\n",
                      strlen("Something goes wrong with getcwd\n"));

                return 0;
        }
        write(fdout, path, strlen(path) + 1);
        write(fdout, "\n", strlen("\n")+1);
        return 1;
}

int pwdL(int fdout) {
        char *path = getenv("PWD");
        if (path == NULL) {
                write(STDERR_FILENO, "Something goes wrong with getenv\n",
                      strlen("Something goes wrong with getenv\n"));
                return 0;
        }
        write(fdout, path, strlen(path) + 1);
        write(fdout, "\n", strlen("\n")+1);
        return 1;
}