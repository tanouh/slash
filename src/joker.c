#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "token.h"
#include "parser.h"
#include "joker.h"
#include "slashLib.h"

char *pattern;

void expand_cmd(char *cmd){

}

int getExtremity(char **basePath, char **followPath, char **argv, int posArg){
        const char *delimiters = "/";
        char *tmp = malloc(strlen(argv[posArg]) + 1);
        if (tmp == NULL) {
                perror("Echec de l'allocation de memoire a tmp\n");
                return -1;
        }
        strcpy(tmp, argv[posArg]);

        int posBeginning = 0;
        int posFollowing;
        if (argv[posArg][0] == '/')
                posFollowing = 0;
        else posFollowing = -1;
        char *expandedPath = strtok(tmp, delimiters);

        int cptSlash = 0;
        while (expandedPath != NULL || cptSlash > 0) {
                if (cptSlash) cptSlash--;
                posBeginning = posFollowing;
                posFollowing += strlen(expandedPath)+1;

                while(posFollowing+1 < strlen(argv[posArg]) &&
                argv[posArg][posFollowing+1] == '/'){
                        cptSlash++;
                        posFollowing++;
                }

                if (expandedPath[0] == '*') break;
                expandedPath = strtok(NULL, delimiters);
        }

        pattern = malloc(strlen(expandedPath));
        if (pattern == NULL){
                free(tmp);
                perror("Echec de l'allocation de memoire a pattern\n");
                return -1;
        }
        strcpy(pattern, expandedPath + 1);

        *followPath = malloc(strlen(argv[posArg]) - posFollowing + 1);
        if (*followPath == NULL){
                free(tmp);
                free(pattern);
                perror("Echec de l'allocation de memoire a followPath\n");
                return -1;
        }
        strcpy(*followPath, argv[posArg] + posFollowing);

        int basePathEmpty = (posBeginning == -1);
        if (basePathEmpty)
                *basePath = ".";
        else {
                *basePath = malloc(posBeginning + 2);
                if (*basePath == NULL) {
                        free(tmp);
                        free(pattern);
                        free(followPath);
                        perror("Echec de l'allocation de memoire a basePath\n");
                        return -1;
                }
                memmove(*basePath, argv[posArg], posBeginning+1);
                memset(*basePath + posBeginning + 1, '\0', 1);
        }
        free(tmp);
        return basePathEmpty;
}

int verifFile(char *basePath, char *followPath, char *fileName){

        if (!strcmp(basePath, ".")) basePath = "./";
        for (int i = 0; i < strlen(followPath); i++){
                if (followPath[i] == '*') return 0;
        }
        char *path = malloc(strlen(basePath) + strlen(fileName) + strlen(followPath) + 1);
        if (path == NULL){
                perror("Echec de l'allocation de memoire a path\n");
                return 1;
        }
        memmove(path, basePath, strlen(basePath));
        memmove(path + strlen(basePath), fileName, strlen(fileName));
        memmove(path + strlen(basePath) + strlen(fileName), followPath, strlen(followPath)+1);

        char *tmp = malloc(strlen(path) + 1);
        if (tmp == NULL) {
                free(path);
                perror("Echec de l'allocation de memoire a tmp\n");
                return 1;
        }
        strcpy(tmp, path);

        const char *delimiters = "/";

        char *lastFile;
        char *cutPath = strtok(tmp, delimiters);
        while (cutPath != NULL) {
                lastFile = cutPath;
                cutPath = strtok(NULL, delimiters);
        }
        char *regex = malloc(strlen(path) - strlen(lastFile)+1);
        if (regex == NULL) {
                free(tmp);
                free(path);
                perror("Echec de l'allocation de memoire a tmp\n");
                return 1;
        }
        memmove(regex, path, strlen(path) - strlen(lastFile));
        memset(regex + strlen(path) - strlen(lastFile), '\0', 1);

        DIR *dir = opendir(regex);
        if (dir == NULL){
                free(tmp);
                free(path);
                free(regex);
                return 1;
        }
        struct dirent *file;

        while ((file = readdir(dir))) {
                if (!strcmp(path + strlen(path) - strlen(lastFile), file->d_name)) {
                        free(dir);
                        free(tmp);
                        free(path);
                        free(regex);
                        return 0;
                }
        }
        free(tmp);
        free(dir);
        free(path);
        free(regex);
        return -1;
}

int filter (const struct dirent *file){
        return (file->d_name[0] != '.' &&
                     !strcmp(pattern, file->d_name + (strlen(file->d_name) - strlen(pattern))));
}


int expand_path(char **argv, token **first, token **last, int posArg, int *nbArg) {

        char *basePath;
        char *followPath;
        int basePathEmpty = getExtremity(&basePath, &followPath, argv, posArg);
        if (basePathEmpty == -1) return 1;

        struct dirent *file;
        int nbFile = 0;
        DIR *dir = opendir(basePath);
        if(dir == NULL){
                if (!basePathEmpty) free(basePath);
                free(followPath);
                return -1;
        }

        int ret_val;
        int argValid = 0;
        while((file = readdir(dir))) {
                if (file->d_name[0] != '.' &&
                    !strcmp(pattern, file->d_name + (strlen(file->d_name) - strlen(pattern)))) {
                        ret_val = verifFile(basePath, followPath, file->d_name);
                        if (ret_val == 1) return 1;
                        if (!ret_val) {
                                argValid = 1;
                                nbFile++;
                        }
                }
        }
        closedir(dir);

        if (!argValid) return -1;

        dir = opendir(basePath);
        file = NULL;
        if(dir == NULL){
                if (!basePathEmpty) free(basePath);
                free(followPath);
                perror("Echec de l'ouverture du repertoire dir\n");
                return -1;
        }

        struct dirent **filesRead = malloc(nbFile * sizeof (struct dirent));
        if(filesRead == NULL){
                if (!basePathEmpty) free(basePath);
                free(followPath);
                perror("Echec de l'allocation de memoire a filesRead\n");
                return 1;
        }

        int k = 0;
        while((file = readdir(dir))){
                if (file->d_name[0] != '.' &&
                !strcmp(pattern, file->d_name + (strlen(file->d_name) - strlen(pattern)))){
                        ret_val = verifFile(basePath, followPath, file->d_name);
                        if (ret_val == 1) {
                                freeTab((void **)filesRead, nbFile);
                                return 1;
                        }
                        if (!ret_val || ret_val == -1) {
                                filesRead[k] = file;
                                k++;
                        }
                }
        }
        closedir(dir);

        scandir(basePath, &filesRead, filter, alphasort);
        if (basePathEmpty) basePath = "";

        int i = 0;
        token *currentTok = *first;
        while (i < posArg-1 && currentTok->next != *last){
                currentTok = currentTok->next;
                i++;
        }
        token *newTok;
        token *tmpTok;

        for (k = 0; k < nbFile; k++){
                newTok = malloc(sizeof(token));
                if (newTok == NULL){
                        free(pattern);
                        freeTab((void **)filesRead, nbFile);
                        if (!basePathEmpty) free(basePath);
                        free(followPath);
                        perror("Echec de l'allocation de memoire a newTok\n");
                        return 1;
                }

                tmpTok = currentTok->next;
                currentTok->next = newTok;
                tmpTok->precedent = newTok;

                newTok->name = malloc(strlen(basePath) + strlen(filesRead[k]->d_name) + strlen(followPath) +1);
                if (newTok->name == NULL){
                        free(pattern);
                        freeTab((void **)filesRead, nbFile);
                        if (!basePathEmpty) free(basePath);
                        free(followPath);
                        perror("Echec de l'allocation de memoire a newTok\n");
                        return 1;
                }

                memmove(newTok->name, basePath, strlen(basePath));
                memmove(newTok->name + strlen(basePath),
                        filesRead[k]->d_name, strlen(filesRead[k]->d_name));
                memmove(newTok->name + strlen(basePath) + strlen(filesRead[k]->d_name),
                        followPath, strlen(followPath) + 1);

                newTok->precedent = currentTok;
                newTok->next = tmpTok;
                newTok->type = ARG;

                currentTok = currentTok->next;
        }

        if (posArg + 1 == *nbArg) {
                tmpTok = (*last)->next;
                *last = (*last)->precedent;
                (*last)->next = tmpTok;
                if (tmpTok != NULL) {
                        tmpTok->precedent = *last;
                        free(tmpTok->name);
                        free(tmpTok);
                }
        }else{
                tmpTok = currentTok->next->next;
                free(tmpTok->precedent->name);
                free(tmpTok->precedent);
                tmpTok->precedent = currentTok;
                currentTok->next = tmpTok;
        }


        free(pattern);
        freeTab((void **)filesRead, nbFile);
        if (!basePathEmpty) free(basePath);
        free(followPath);
        *nbArg = *nbArg + nbFile - 1;
        return 0;
}
