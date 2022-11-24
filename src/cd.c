#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cd.h"

extern char * lastWd;

int exec_cd(int argc, char *argv[]) {
//        printf("%s\n", lastWd);
        if (argc == 0) {
                return cd(getenv("HOME"));
        }
        if (argc > 2) {
                write(STDERR_FILENO, "-slash: too many arguments \n", strlen("-slash: too many arguments \n"));
                return 1;
        }
        if (argc == 1){
                if (!strcmp((argv[0]), "-"))
                        return cd(lastWd);

                else if(!strcmp((argv[0]), "-L"))
                        return cd(argv[1]);

                else if (!strcmp((argv[0]), "-P"))
                        return cd(argv[1]);

                else
                        return cd(argv[0]);
        }

        write(STDERR_FILENO, "-slash: invalid option1 \n", strlen("-slash: invalid option1 \n"));

        return 1;

}

/*
int cdP(char *path) {
    char * buff= malloc (sizeof(buff)*256);
    getcwd(buff, 256);
    strcat(buff, path);
    if (setenv("PWD", buff, 1) == 0) {
        write(STDERR_FILENO, "c'est bien passé\n",
              strlen("c'est bien passé\n"));
        pwdP(STDERR_FILENO);

        return 0;
    }
    write(STDERR_FILENO, "Something goes wrong with chdir\n",
          strlen("Something goes wrong with chdir\n"));

    return 1;
}
*/

//le path doit pas contenir des / au debut, sauf pour absolue sinon segmentation fault
int cd(char *path) {
        char *buff = clean(path);
        if (open(buff, O_RDONLY) != -1) {
                strcpy(lastWd, getenv("PWD"));
                if (setenv("PWD", buff, 1) == 0) return 0;
        }
        write(STDERR_FILENO, "Something goes wrong with cd\n",
              strlen("Something goes wrong with cd\n"));

        return 1;
}

char *clean(char *path) {


        char *pwd = malloc(strlen(getenv("PWD")));
        if (pwd == NULL){
                write(STDERR_FILENO, "Echec de l'allocation a pwd\n", strlen("Echec de l'allocation a pwd\n"));
                return NULL;
        }
        strcpy(pwd, getenv("PWD"));

        if (path[0] == '/') {
                memset(pwd, 0x0, 1);
        }

        int count = strlen(pwd) + strlen(path) + 2;

//    printf("%s PWD\n",pwd);
//    printf("%s PATH\n",path);


        size_t pwdv_size, pathv_size, res_size;

//    printf(" Pwd: \n");

        char **pwdv = cut(pwd, &pwdv_size);

//    printf(" Path: \n");
        char **pathv = cut(path, &pathv_size);

        res_size = pwdv_size + pathv_size;
        char **result = malloc(sizeof(char *) * res_size);

        memmove(result, pwdv, pwdv_size * sizeof(char *));

        for (size_t k = 0; k < pathv_size; k++) {
                if (!strcmp(pathv[k], "..")) {
                        free(result[--pwdv_size]);
                        result[pwdv_size] = NULL;
                        count -= 3;
                        res_size -= 2;
                } else if (!strcmp(pathv[k], ".")) {
                        count -= 2;
                        res_size -= 1;
                } else {
                        result[pwdv_size] = pathv[k];
                        pwdv_size++;
                }
        }

        char *res = malloc(count);
        memset(res, 0x0, count);

        size_t h = 0;
        for (size_t k = 0; k < res_size; k++) {
                if (result[k]) {
                        size_t rs = strlen(result[k]);
//            printf("%s %ld\n",result[k], rs);
                        memset(res + h, '/', 1);
                        memmove(res + h + 1, result[k], rs);
                        h += rs + 1;
                }
        }

//    printf("%s\n", res);

        free(pathv);
        free(pwdv);
        return res;
}

char **cut(char *path, size_t *size) {
        int i = 0;
        size_t path_s = strlen(path);

        char *npath = path;
        int norm = path[path_s - 1] != '/';

        if (norm) {
                npath = malloc(path_s + 2);
                memmove(npath, path, path_s);

                npath[path_s] = '/';
                npath[path_s + 1] = 0x0;
        }

//    printf("%s\n", npath);

        for (size_t k = 1; k < path_s + 1; k++) {
                if (npath[k] == '/') i++;
        }

        *size = i;

        int a = (npath[0] == '/') ? 1 : 0;
        int b = 0;

        char **pathv = malloc(sizeof(char *) * i);

//    printf("%d  ",i);

        for (size_t k = 0; k < path_s + 1 && b < i; k++) {
                if (npath[k] == '/' && k) {
                        pathv[b] = malloc(k - a + 1);
                        memset(pathv[b], 0x0, k - a + 1);
                        memmove(pathv[b], npath + a, k - a);
//            printf("%s  ",pathv[b]);
//            printf("%d  ",b);


                        a = k + 1;
                        b++;
                }
        }
//    printf(" \n");

        if (norm) free(npath);

        return pathv;
}

char *removeFirstSlash(char *path) {
        if (path[0] == '/') {
                char *path1 = malloc(strlen(path) * sizeof(char *) - 1);
                memmove(path1, path + 1, strlen(path) * sizeof(char *) - 1);
                return path1;
        }
        return path;
}

char *addLastSlash(char *path) {
        if (path[strlen(path) - 1] != '/') {
                char *path1 = malloc(strlen(path) * sizeof(char *));
                memmove(path1, '/' + path, strlen(path) * sizeof(char *) + 1);
                return path1;
        }
        return path;
}
