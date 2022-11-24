#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cd.h"
#include "pwd.h"


extern char *lastWd;

int exec_cd(int argc, char *argv[]) {
    if (argc == 0) {
        return cd(getenv("HOME"), 0);
    }
    if (argc > 2) {
        write(STDERR_FILENO, "-slash: too many arguments \n", strlen("-slash: too many arguments \n"));
        return 1;
    }
    if (argc == 1) {
        if (!strcmp(argv[0], "-"))
            return cd(lastWd, 0);
        else
            return cd(argv[0], 0);
    }
    if (argc == 2) {
        if (!strcmp(argv[0], "-L"))
            return cd(argv[1], 0);

        else if (!strcmp(argv[0], "-P"))
            return cd(argv[1], 1);
        else
            return 1;

    }
    write(STDERR_FILENO, "-slash: invalid option1 \n", strlen("-slash: invalid option1 \n"));

    return 1;

}

int cd(char *path, int physical) {
    char *buff;
    char resolved_path[PHYS_PATH_LEN];
    char *envpath;
    if (physical) {
        envpath = realpath(getenv("PWD"), resolved_path);
        buff = clean(path, envpath);
        char new_wd[PHYS_PATH_LEN];
        if (chdir(buff) == -1) {
            write(STDERR_FILENO, "-slash : cd : Something goes wrong with cd\n",
                  strlen("-slash : cd : Something goes wrong with cd\n"));
            return 1;
        }
        strcpy(lastWd, getenv("PWD"));
        setenv("PWD", getcwd(new_wd, PHYS_PATH_LEN), 1);
        free(buff);
        return 0;

    } else {
        envpath = getenv("PWD");
        buff = clean(path, envpath);
        if (open(buff, O_RDONLY) != -1) {
            strcpy(lastWd, envpath);
            if (setenv("PWD", buff, 1) == 0) {
                free(buff);
                return 0;
            }
        }
        return cd(path, 1);
    }
    write(STDERR_FILENO, "-slash : cd : Something goes wrong with cd\n",
          strlen("-slash : cd : Something goes wrong with cd\n"));

    return 1;
}

char *clean(char *path, char *realpath) {


    char *pwd = malloc(strlen(realpath));
    if (pwd == NULL) {
        write(STDERR_FILENO, "Echec de l'allocation a pwd\n", strlen("Echec de l'allocation a pwd\n"));
        return NULL;
    }
    strcpy(pwd, realpath);

    if (path[0] == '/') {
        memset(pwd, 0x0, 1);
    }

    int count = strlen(pwd) + strlen(path) + 2;

    size_t pwdv_size, pathv_size, res_size;

    char **pwdv = cut(pwd, &pwdv_size);

    char **pathv = cut(path, &pathv_size);

    res_size = pwdv_size + pathv_size;
    char **result = malloc(sizeof(char *) * res_size);
    if (result == NULL) {
        write(STDERR_FILENO, "Echec de l'allocation \n", strlen("Echec de l'allocation\n"));
        return NULL;
    }

    memmove(result, pwdv, pwdv_size * sizeof(char *));

    for (size_t k = 0; k < pathv_size; k++) {
        if (!strcmp(pathv[k], "..")) {
            free(result[--pwdv_size]);
            free(pathv[k]);
            result[pwdv_size] = NULL;
            count -= 3;
            res_size -= 2;
        } else if (!strcmp(pathv[k], ".")) {
            count -= 2;
            res_size -= 1;
            free(pathv[k]);
        } else {
            result[pwdv_size] = pathv[k];
            pwdv_size++;
        }
    }

    char *res = malloc(count);

    memset(res, 0x0, count);
    if (result == NULL) {
        write(STDERR_FILENO, "Echec de l'allocation \n", strlen("Echec de l'allocation\n"));
        return NULL;
    }
    size_t h = 0;
    for (size_t k = 0; k < res_size; k++) {
        if (result[k]) {
            size_t rs = strlen(result[k]);
            memset(res + h, '/', 1);
            memmove(res + h + 1, result[k], rs);
            free(result[k]);
            h += rs + 1;
        }
    }
    free(pwd);
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

    for (size_t k = 1; k < path_s + 1; k++) {
        if (npath[k] == '/') i++;
    }

    *size = i;

    int a = (npath[0] == '/') ? 1 : 0;
    int b = 0;

    char **pathv = malloc(sizeof(char *) * i);

    for (size_t k = 0; k < path_s + 1 && b < i; k++) {
        if (npath[k] == '/' && k) {
            pathv[b] = malloc(k - a + 1);
            memset(pathv[b], 0x0, k - a + 1);
            memmove(pathv[b], npath + a, k - a);


            a = k + 1;
            b++;
        }
    }

    if (norm) free(npath);
    return pathv;
}
/*
void free_absolute(char **path, size_t *path_s){
    for (size_t b = 0; b < path_s ; b++) {
        free(path[b]);
    }
}*/