#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cd.h"


int parser_cd(int argc, char *argv[]) {
    if (argc == 2) {
        return cd(argv[1]);
    }
    if (argc > 4) {
        write(STDERR_FILENO, "-slash: too many arguments \n", strlen("-slash: too many arguments \n"));
        return 1;
    }

    if (argc <= 1) {
        write(STDERR_FILENO, "-slash: invalid option \n", strlen("-slash: invalid option \n"));
        return 1;
    }

    if ((argv[1]) && (!strcmp((argv[1]), "-L"))) {

        return cd(argv[2]);
    }
    if ((argv[1]) && (!strcmp((argv[1]), "-P"))) {
        return cd(argv[2]);
    }
    write(STDERR_FILENO, "-slash: invalid option \n", strlen("-slash: invalid option \n"));

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
    if ((open(buff, O_RDONLY) != -1) && (setenv("PWD", buff, 1) == 0)) {
        return 0;
    }
    write(STDERR_FILENO, "Something goes wrong with cd\n",
          strlen("Something goes wrong with cd\n"));

    return 1;
}

char *clean(char *path) {
    char *pwd = getenv("PWD");
    int count = strlen(pwd) + strlen(path);

    size_t pwdv_size, pathv_size, res_size;

    char **pathv = cut(path, &pathv_size);
    char **pwdv = cut(pwd, &pwdv_size);

    res_size = pwdv_size + pathv_size;
    char **result = malloc(sizeof(char *) * res_size);

    memmove(result, pwdv, pwdv_size * sizeof(char *));

    for (size_t k = 0; k < pathv_size; k++) {
        if (!strcmp(pathv[k], "..")) {
            free(result[--pwdv_size]);
            result[pwdv_size] = NULL;
            count -= 3;
        } else if (!strcmp(pathv[k], ".")) {
            count -= 2;
        } else {
            result[pwdv_size - 1] = pathv[k];
            pwdv_size++;
        }
    }

    char *res = malloc(count);
    size_t h = 0;
    for (size_t k = 0; k < res_size; k++) {
        if (result[k]) {
            size_t rs = strlen(result[k]);
            memset(res + h, '/', 1);
            memmove(res + h + 1, result[k], rs);
            h += rs + 1;
        }
    }

    printf(" %s\n", res);

    free(pathv);
    free(pwdv);
    return res;
}

char **cut(char *path, size_t *size) {
    int i = 0;
    size_t path_s = strlen(path);
    for (size_t k = 0; k < path_s; k++) {
        if (path[k] == '/') i++;
    }

    *size = i;
    char **pathv = malloc(sizeof(char *) * i);

    int a = path[0] == '/';
    int b = 0;
    for (size_t k = 0; k < path_s && b < i; k++) {
        if (path[k] == '/' && k) {
            pathv[b] = malloc(k - a + 1);
            memset(pathv[b], 0x0, k - a + 1);
            memmove(pathv[b], path + a, k - a);
            a = k + 1;
            b++;
        }
    }

    return pathv;
}


