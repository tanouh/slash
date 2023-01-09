#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "pwd.h"
#include "slasherr.h"

int exec_pwd(int *fdin, int *fdout, int *fderr, int argc, char *argv[]) {
        if (argc == 1) {
                free(argv);
                return pwdL(*fdout);
        }
        //Si on doit faire pwd retourne meme si les argument ne sont pas valide enlever cette ligne et l'autre signalé
        if (argc > 2) {
                print_err("pwd","too many arguments");
                free(argv);
                return 1;
        }

        if ((argc == 2) && (!strcmp((argv[1]), "-L"))) {
                free(argv);
		return pwdL(*fdout);
        }
        if ((argc == 2) && (!strcmp((argv[1]), "-P"))) {
                free(argv);
		return pwdP(*fdout);
        }
        //Celle-ci
	free(argv);
	print_err("pwd", "invalid option");
        return 1;
}

int pwdP(int fdout) {
	char buff [PHYS_PATH_LEN];
        char *path = realpath(getenv("PWD"),buff);
        // if (getcwd(path, PHYS_PATH_LEN) == NULL) {
	if( path == NULL){
		print_err("pwd", "Something went wrong");

                return 1;
        }
        write(fdout, path, strlen(path));
        write(fdout, "\n", strlen("\n"));
        return 0;
}

int pwdL(int fdout) {
        char *path = getenv("PWD");
	
        if (path == NULL) {
                print_err("pwd", "Something went wrong");

                return 1;
        }
        write(fdout, path, strlen(path));
        write(fdout, "\n", strlen("\n"));
        return 0;
}