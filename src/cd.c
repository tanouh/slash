#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "cd.h"
#include "pwd.h"
#include "slash.h"
#include "slasherr.h"

int exec_cd(int fdin, int fdout, int fderr, int argc, char *argv[]) {
        if (argc == 1) {
                free(argv);
                return cd(getenv("HOME"), 0);
        }
        if (argc > 3) {
                free(argv);
                print_err("cd", "too many arguments");
                return 1;
        }
        if (argc == 2) {
                if (!strcmp(argv[1], "-")) {
                        free(argv);
                        return cd(lastWd, 0);
                } else {
                        char *tmp = argv[1];
                        free(argv);
                        return cd(tmp, 0);
                }

        }
        if (argc == 3) {
                char *tmp = argv[2];
                if (!strcmp(argv[1], "-L")) {
                        free(argv);
                        return cd(tmp, 0);
                } else if (!strcmp(argv[1], "-P")) {
                        free(argv);
                        return cd(tmp, 1);
                } else {
                        free(argv);
                        return 1;
                }
        }
        free(argv);
        print_err("cd", "invalid option");

        return 1;
}

int cd(char *path, int physical) {

        char *buff;
        char resolved_path[PHYS_PATH_LEN];
        char *envpath;
        if (physical) {

                envpath = realpath(getenv("PWD"), resolved_path);

                buff = catPath(path, envpath);

                char new_wd[PHYS_PATH_LEN];

                if (chdir(buff) == -1) {
                        free(buff);
                        print_err("cd", "Couldn't change the directory");

                        return 1;
                }

                free(buff);
                buff = clean(path, envpath);

                memset(lastWd, 0x0, MAX_ARGS_STRLEN);
                memmove(lastWd, getenv("PWD"), strlen(getenv("PWD")));

                if (setenv("PWD", getcwd(new_wd, PHYS_PATH_LEN), 1) == 0) {
                        free(buff);
                        return 0;
                }

                free(buff);
                return 1;
        } else {
                envpath = getenv("PWD");
                buff = clean(path, envpath);
                if (open(buff, O_RDONLY) != -1) {
                        if (chdir(buff) == -1) {
                                free(buff);
                                print_err("cd", "Couldn't change the directory");
                                return 1;
                        }
                        memset(lastWd, 0x0, MAX_ARGS_STRLEN);
                        memmove(lastWd, envpath, strlen(envpath));

                        if (setenv("PWD", buff, 1) == 0) {
                                free(buff);
                                return 0;
                        }
                        free(buff);
                        return 1;
                }

                free(buff);
                return cd(path, 1);
        }
        print_err("cd", "Something goes wrong with cd");

        return 1;
}

char *catPath(char *path, char *realpath) {

        char *buff;

        if (path[0] == '/') {
                buff = malloc(strlen(path) + 1);
                if (buff == NULL) {
                        print_err("cd", MALLOC_ERR);
                        return NULL;
                }
                strcpy(buff, path);
                return buff;
        }

        buff = malloc(strlen(path) + strlen(realpath) + 2);
        if (buff == NULL) {
                print_err("cd", MALLOC_ERR);
                return NULL;
        }

        size_t size = strlen(realpath);
        memmove(buff, realpath, size);
        memset(buff + size, '/', 1);
        memmove(buff + size + 1, path, strlen(path) + 1);
        return buff;
}

/**
 *
 * @param path where you want to go
 * @param realpath is the pwd
 * @return a normalized path
 *
 * Example = clean ("I/like/./ice-cream/../../", "pwd") = "pwd/I"
 */

char *clean(char *path, char *realpath) {

        char *pwd = malloc(PHYS_PATH_LEN);
        if (pwd == NULL) {
                print_err("cd", MALLOC_ERR);
                return NULL;
        }
        memmove(pwd, realpath, strlen(realpath) + 1);

        if (path[0] == '/') {
                memset(pwd, 0x0, strlen(pwd));
        }

        int count = strlen(pwd) + strlen(path) + 2;

        size_t pwdv_size, pathv_size, res_size;

        char **pwdv = cut(pwd, strlen(pwd), &pwdv_size);

        char **pathv = cut(path, strlen(path), &pathv_size);

        res_size = pwdv_size + pathv_size;
        char **result = malloc(sizeof(char *) * res_size);
        if (result == NULL) {
                print_err("cd", MALLOC_ERR);
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
        if (res == NULL) {
                print_err("cd", MALLOC_ERR);
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
        free(result);
        return res;
}

/**
 *
 * @param path
 * @param size
 * @return a char ** with each part of the path divided by an "/"
 *
 * Example = cut("I/like/ice-cream",3) = [["I"],["like"], ["ice-cream"]]
 */

char **cut(char *path, size_t path_s, size_t *size) {
        int i = 0;
        char *npath = path;
        int norm = 1;
        if (path_s > 0)
                norm = path[path_s - 1] != '/';

        if (norm) {
                npath = malloc(path_s + 2);
                if (npath == NULL) {
                        print_err("cd", MALLOC_ERR);
                        return NULL;
                }
                memmove(npath, path, path_s);

                npath[path_s] = '/';
                npath[path_s + 1] = 0x0;
        }

        for (size_t k = 1; k < path_s + 1; k++) {
                if (npath[k] == '/')
                        i++;
        }

        *size = i;

        int a = (npath[0] == '/') ? 1 : 0;
        int b = 0;

        char **pathv = malloc(sizeof(char *) * i);
        if (pathv == NULL) {
                free(npath);
                print_err("cd", MALLOC_ERR);
                return NULL;
        }

        for (size_t k = 0; k < path_s + 1 && b < i; k++) {
                if (npath[k] == '/' && k) {
                        pathv[b] = malloc(k - a + 1);
                        memset(pathv[b], 0x0, k - a + 1);
                        memmove(pathv[b], npath + a, k - a);

                        a = k + 1;
                        b++;
                }
        }

        if (norm)
                free(npath);
        return pathv;
}
