#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "token.h"
#include "parser.h"
#include "joker.h"
#include "slashLib.h"

char *pattern;
int basePathEmpty;
int followPathEmpty;

int getExtremity(char **basePath, char **followPath, token *current){
        const char *delimiters = "/";
        char *tmp = malloc(strlen(current->name) + 1);
        if (tmp == NULL) {
                perror("Echec de l'allocation de memoire a tmp\n");
                return -1;
        }
        strcpy(tmp, current->name);

        int posBeginning = 0; //position du debut de la partie du chemin a traiter
        int posFollowing; //position de debut de la partie du chemin suivante
        if (current->name[0] == '/')
                posFollowing = 0;
        else posFollowing = -1;
        char *expandedPath = strtok(tmp, delimiters);

        int cptSlash = 0; //compte les '/' successifs
        int cptEtoile = 0; //compte les etoiles
        while (expandedPath != NULL || cptSlash > 0) {
                if (cptSlash) cptSlash--;
                posBeginning = posFollowing;
                posFollowing += strlen(expandedPath)+1;

                while(posFollowing+1 < strlen(current->name) &&
                current->name[posFollowing+1] == '/'){
                        cptSlash++;
                        posFollowing++;
                }

                if (expandedPath[0] == '*' && ++cptEtoile > current->nbEtoileNom ) break;
                expandedPath = strtok(NULL, delimiters);
        }

        pattern = malloc(strlen(expandedPath)); // suffixe des fichiers definis par l'*
        if (pattern == NULL){
                free(tmp);
                perror("Echec de l'allocation de memoire a pattern\n");
                return -1;
        }
        strcpy(pattern, expandedPath + 1);

        *followPath = malloc(strlen(current->name) - posFollowing + 1); //chemin suivant le fichier defini par l'*
        if (*followPath == NULL){
                free(tmp);
                free(pattern);
                perror("Echec de l'allocation de memoire a followPath\n");
                return -1;
        }
        strcpy(*followPath, current->name + posFollowing);

        basePathEmpty = (posBeginning == -1);
        if (basePathEmpty) // si le chemin precedant l'etoile est vide
                *basePath = ".";
        else {
                *basePath = malloc(posBeginning + 2); // chemin precedant l'etoile
                if (*basePath == NULL) {
                        free(tmp);
                        free(pattern);
                        free(followPath);
                        perror("Echec de l'allocation de memoire a basePath\n");
                        return -1;
                }
                memmove(*basePath, current->name, posBeginning+1);
                memset(*basePath + posBeginning + 1, '\0', 1);
        }
        free(tmp);
        return 0;
}

int verifFile(char *basePath, char *followPath, char *fileName, token *current){
        if (!strcmp(basePath, ".")) basePath = "./"; //cas ou le chemin precedant l'* est vide
        if (!strcmp(followPath, "/")) { //cas ou le chemin suivant le fichier defini par l'* est vide
                followPathEmpty = 1;
                followPath = "";
        }
        else followPathEmpty = 0;

        char *pathFile = malloc(strlen(basePath) + strlen(fileName) + 1); //chemin du fichier a traiter
        if (pathFile == NULL){
                perror("Echec de l'allocation de memoire a pathFile\n");
                if (followPathEmpty) followPath = "/";
                return 1;
        }
        memmove(pathFile, basePath, strlen(basePath));
        memmove(pathFile + strlen(basePath), fileName, strlen(fileName) + 1);


        struct stat *st = malloc(sizeof(struct stat));
        if (st == NULL) {
                free(pathFile);
                if (followPathEmpty) followPath = "/";
                perror("Echec de l'allocation de memoire a st\n");
                return -1;
        }
        int ret_val = lstat(pathFile, st); //on stocke dans st les infos sur le fichier a traiter
        if (ret_val == -1) {
                free(pathFile);
                if (followPathEmpty) followPath = "/";
                free(st);
                return -1;
        }

        if ((current->nbEtoileFrom2 > 0 && S_ISLNK(st->st_mode)) || //cas ou au moins la premiere * est issue
                                                                    //de ** : on ne rentre pas dans les chemin sym
                (!S_ISDIR(st->st_mode) && !S_ISLNK(st->st_mode)
                && (strcmp(followPath, "") || followPathEmpty ))){ //cas ou le chemin n'est ni un repertoire ni un
                                                                   //chemin sym alors que le chemin suffixe
                                                                   //n'est pas vide

                free(pathFile);
                if (followPathEmpty) followPath = "/";
                free(st);
                return -1;
        }
        free(st);
        free(pathFile);

        //on cherche la premiere etoile qui ne fasse pas partie d'un nom de fichier
        for (int i = 0; i < strlen(fileName); i++){
                if (fileName[i] == '*') {
                        if (!current->currentEtoileNom) {
                                current->nbEtoileNom++;
                                current->currentEtoileNom = 1;
                                break;
                        }

                }
        }

        //on verifie que le chemin suffixe ne contient pas d'etoile
        //s'il en contient une, on ne peut pas determiner dans cette iteration si
        //ce fichier est valide
        for (int i = 0; i < strlen(followPath); i++){
                if (followPath[i] == '*') {
                        if (followPathEmpty) followPath = "/";
                        return 0;
                }
        }

        char *path = malloc(strlen(basePath) + strlen(fileName) + strlen(followPath) + 1);
        //chemin en remplacant l'* par le nom du fichier
        if (path == NULL){
                perror("Echec de l'allocation de memoire a path\n");
                if (followPathEmpty) followPath = "/";
                if (current->currentEtoileNom) {
                        current->nbEtoileNom--;
                        current->currentEtoileNom = 0;
                }
                return 1;
        }
        memmove(path, basePath, strlen(basePath));
        memmove(path + strlen(basePath), fileName, strlen(fileName));
        memmove(path + strlen(basePath) + strlen(fileName), followPath, strlen(followPath)+1);

        char *tmp = malloc(strlen(path) + 1);
        if (tmp == NULL) {
                free(path);
                if (followPathEmpty) followPath = "/";
                if (current->currentEtoileNom) {
                        current->nbEtoileNom--;
                        current->currentEtoileNom = 0;
                }
                perror("Echec de l'allocation de memoire a tmp\n");
                return 1;
        }
        strcpy(tmp, path);

        const char *delimiters = "/";

        //On cherche a entrer dans le repertoire contenant le dernier fichier du chemin,
        //pour verifier que ce fichier y est
        //S'il y est, le chemin est valide
        char *lastFile; //dernier fichier du chemin
        char *beforeLastFile; //avant-dernier fichier du chemin
        char *cutPath = strtok(tmp, delimiters);
        while (cutPath != NULL) {
                beforeLastFile = lastFile;
                lastFile = cutPath;
                cutPath = strtok(NULL, delimiters);
        }
        if (strlen(followPath) > 0 && followPath[strlen(followPath)-1] == '/'){
                lastFile = beforeLastFile;
        }
        char *regex = malloc(strlen(path) - strlen(lastFile)+1); //chemin sans le dernier fichier
        if (regex == NULL) {
                free(tmp);
                free(path);
                if (followPathEmpty) followPath = "/";
                if (current->currentEtoileNom) {
                        current->nbEtoileNom--;
                        current->currentEtoileNom = 0;
                }
                perror("Echec de l'allocation de memoire a tmp\n");
                return 1;
        }
        memmove(regex, path, strlen(path) - strlen(lastFile));
        memset(regex + strlen(path) - strlen(lastFile), '\0', 1);

        DIR *dir = opendir(regex); //on ouvre le repertoire
        if (dir == NULL){
                free(tmp);
                free(path);
                free(regex);
                if (followPathEmpty) followPath = "/";
                if (current->currentEtoileNom) {
                        current->nbEtoileNom--;
                        current->currentEtoileNom = 0;
                }
                return 1;
        }
        struct dirent *file;

        while ((file = readdir(dir))) {
                //pour chaque fichier du repertoire, on regarde s'il s'agit de celui qu'on cherche
                if (!strcmp(path + strlen(path) - strlen(lastFile), file->d_name)) {
                        free(dir);
                        free(tmp);
                        free(path);
                        free(regex);
                        if (followPathEmpty) followPath = "/";
                        return 0;
                }
        }
        free(tmp);
        free(dir);
        free(path);
        free(regex);
        if (followPathEmpty) followPath = "/";
        if (current->currentEtoileNom) {
                current->nbEtoileNom--;
                current->currentEtoileNom = 0;
        }
        return -1;
}

int openFile(char *path, DIR *dir, int depth){

        struct stat* st = malloc(sizeof(struct stat));
        if (st == NULL){
                perror("Echec de l'allocation de memoire a st\n");
                return -1;
        }
        int ret_val = lstat(path, st); //on stocke les infos du chemin a ouvrir
        if (ret_val == -1) {
                free(st);
                return -1;
        }
        if (!S_ISDIR(st->st_mode) && !S_ISLNK(st->st_mode)){ //on verifie que c'est bien un repertoire
                free(st);
                return -1;
        }

        char *basePath; //chemin du fichier
        struct dirent *file; //fichier
        DIR *tmpDir; //repertoire temporaire
        int maxDepth = depth; //profondeur max de l'arborescence

        //pour chaque fichier, on verifie que l'on peut entrer dedans, et si oui on appelle
        //la fonction dessus recursivement
        while ((file = readdir(dir))){
                if (!strcmp(file->d_name, "..") || !strcmp(file->d_name, ".")) continue;


                basePath = malloc(strlen(path) + strlen(file->d_name) + 2);
                if (basePath == NULL){
                        perror("Echec de l'allocation de memoire a basePath\n");
                        return -1;
                }
                memmove(basePath, path, strlen(path));
                memset(basePath+strlen(path), '/', 1);
                memmove(basePath + strlen(path) + 1, file->d_name, strlen(file->d_name) + 1);

                int ret_val = lstat(basePath, st); // on stocke les infos sur le fichier
                if (ret_val == -1) {
                        free(st);
                        free(basePath);
                        return -1;
                }

                if (S_ISDIR(st->st_mode)){      //on verifie que c'est un repertoire
                        tmpDir = opendir(basePath);
                        if (tmpDir == NULL){
                                free(st);
                                free(basePath);
                                return -1;
                        }
                        ret_val = openFile(basePath, tmpDir, depth+1); // on appelle la fonction recursivement
                        closedir(tmpDir);
                        if (ret_val > maxDepth) maxDepth = ret_val; //on stocke la nouvelle profondeur max
                }
                free(basePath);
        }
        free(st);
        return maxDepth;
}

int expand_path(char **argv, struct tokenList **tokList, int posArg, int *nbArg, enum tokenType type) {

        //on recupere le token correspondant au chemin que l'on traite
        int i = 0;
        token *currentTok = (*tokList)->first;
        while (i < posArg){
                currentTok = currentTok->next;
                i++;
        }

        char *basePath; //chemin precedent l'*
        char *followPath; //chemin suivant le fichier defini par l'*
        getExtremity(&basePath, &followPath, currentTok); // on recupere les valeurs des deux var precedentes
        if (basePathEmpty == -1)  {
                free(pattern);
                return -1;
        }

        struct dirent *file; // le(s) fichier(s) pointe(s) par l'*
        int nbFile = 0; //le nombre de fichier qui correspondent
        DIR *dir = opendir(basePath); //le repertoire contenant les fichiers pointes par l'*
        if(dir == NULL){
                if (!basePathEmpty) free(basePath);
                free(followPath);
                free(pattern);
                return -1;
        }


        int ret_val;
        int argValid = 0; //un booleen qui verifie qu'au moins un fichier est valide
        //pour chaque fichier dans le repertoire, on verifie qu'il peut s'agir d'un des fichiers pointes
        //par l'*, et si oui on incremente nbFile
        while((file = readdir(dir))) {
                if (file->d_name[0] != '.' &&
                    !strcmp(pattern, file->d_name + (strlen(file->d_name) - strlen(pattern)))) {
                        ret_val = verifFile(basePath, followPath, file->d_name, currentTok);
                        if (ret_val == 1)  {
                                closedir(dir);
                                free(pattern);
                                if (!basePathEmpty) free(basePath);
                                free(followPath);
                                return -1;
                        }
                        if (!ret_val) {
                                argValid = 1;
                                nbFile++;
                        }
                }
        }
        closedir(dir);

        //si aucun argument est valide, on laisse le token tel quel et on passe au suivant
        if (!argValid) {
                free(pattern);
                if (!basePathEmpty) free(basePath);
                free(followPath);
                return -1;
        }

        dir = opendir(basePath);
        file = NULL;
        if(dir == NULL){
                if (!basePathEmpty) free(basePath);
                free(followPath);
                free(pattern);
                perror("Echec de l'ouverture du repertoire dir\n");
                return -1;
        }

        struct dirent **filesRead = malloc(nbFile * sizeof (struct dirent));
        if(filesRead == NULL){
                closedir(dir);
                if (!basePathEmpty) free(basePath);
                free(pattern);
                free(followPath);
                perror("Echec de l'allocation de memoire a filesRead\n");
                return -1;
        }

        int k = 0;
        //on recupere a nouveau les fichiers qui peuvent etre pointes par l'*, et on les stocke dans filesRead
        while((file = readdir(dir))){
                if (file->d_name[0] != '.' &&
                !strcmp(pattern, file->d_name + (strlen(file->d_name) - strlen(pattern)))){
                        ret_val = verifFile(basePath, followPath, file->d_name, currentTok);
                        if (ret_val == 1) {
                                free(filesRead);
                                free(pattern);
                                if (!basePathEmpty) free(basePath);
                                free(followPath);
                                closedir(dir);
                                return -1;
                        }
                        if (!ret_val) {
                                filesRead[k] = file;
                                k++;
                        }
                }
        }


        if (basePathEmpty) basePath = "";

        //on se place sur le token precedent celui qu'on considere
        i = 0;
        currentTok = (*tokList)->first;
        while (i < posArg-1 && currentTok->next != (*tokList)->last){
                currentTok = currentTok->next;
                i++;
        }
        int nbEtoileFrom2 = 0; //nombre d'* issues de **
        int nbEtoileNom = 0; //nb de fichiers contenant une * dans leur nom
        if (currentTok->next != NULL) {
                nbEtoileFrom2 = currentTok->next->nbEtoileFrom2;
                nbEtoileNom = currentTok->next->nbEtoileNom;
                currentTok->next->currentEtoileNom = 0;
        }

        token *newTok;
        token *tmpTok = NULL;

        //pour chaque fichier de filesRead, on cree un nouveau token et on l'ajoute a la tokenList
        for (int k = 0; k < nbFile; k++){
                newTok = malloc(sizeof(token));
                if (newTok == NULL){
                        free(pattern);
                        free(filesRead);
                        if (!basePathEmpty) free(basePath);
                        free(followPath);
                        closedir(dir);
                        perror("Echec de l'allocation de memoire a newTok\n");
                        return -1;
                }

                if (type == ARG) {
                        tmpTok = currentTok->next;
                        currentTok->next = newTok;
                        if (tmpTok == NULL)
                                (*tokList)->last = newTok;
                        else
                                tmpTok->precedent = newTok;
                        newTok->precedent = currentTok;
                        newTok->next = tmpTok;
                        newTok->type = ARG;
                }else if (type == CMD){
                        tmpTok = currentTok->precedent;
                        if (tmpTok == NULL)
                                (*tokList)->first = newTok;
                        else
                                tmpTok->next = newTok;
                        currentTok->precedent = newTok;
                        newTok->precedent = tmpTok;
                        newTok->next = currentTok;
                        newTok->type = CMD;
                }

                newTok->name = malloc(strlen(basePath) + strlen(filesRead[k]->d_name) + strlen(followPath) +1);
                if (newTok->name == NULL){
                        free(pattern);
                        free(filesRead);
                        if (!basePathEmpty) free(basePath);
                        free(followPath);
                        closedir(dir);
                        perror("Echec de l'allocation de memoire a newTok\n");
                        return -1;
                }

                memmove(newTok->name, basePath, strlen(basePath));
                memmove(newTok->name + strlen(basePath),
                        filesRead[k]->d_name, strlen(filesRead[k]->d_name));
                memmove(newTok->name + strlen(basePath) + strlen(filesRead[k]->d_name),
                        followPath, strlen(followPath) + 1);
                newTok->nbEtoileFrom2 = nbEtoileFrom2 - 1;
                if (nbEtoileNom > 0)
                        newTok->nbEtoileNom = nbEtoileNom-1;
                else
                        newTok->nbEtoileNom = 0;

                for (int i = 0; i < strlen(newTok->name) - strlen(followPath); i++) {
                        if (newTok->name[i] == '*') {
                                newTok->nbEtoileNom = nbEtoileNom;
                                break;
                        }
                }
                newTok->currentEtoileNom = 0;

                currentTok = currentTok->next;
        }

        tmpTok = NULL;
        //on supprime le token qui a servi de base
        if (type == CMD && currentTok != NULL)
                freeToken(*tokList, currentTok->precedent);
        else if (currentTok != NULL)
                freeToken(*tokList, currentTok->next);
        closedir(dir);

        free(pattern);
        free(filesRead);
        if (!basePathEmpty) free(basePath);
        free(followPath);
        *nbArg = *nbArg + nbFile - 1;
        return 0;
}

int expand_double(char **argv, struct tokenList **tokList, int posArg, int *nbArg, enum tokenType type){

        //On recupere le token correspondant au chemin que l'on traite
        int i = 0;
        token *currentTok = (*tokList)->first;
        while (i < posArg){
                currentTok = currentTok->next;
                i++;
        }

        char *basePath; //chemin precedent l'*
        char *followPath; //chemin suivant le fichier defini par l'*
        getExtremity(&basePath, &followPath, currentTok); // on recupere les valeurs des deux var precedentes
        if (basePathEmpty == -1)  {
                free(pattern);
                return -1;
        }

        DIR *dir = opendir(basePath); //le racine de l'arborescence que l'on va considerer
        if(dir == NULL){
                if (!basePathEmpty) free(basePath);
                free(followPath);
                free(pattern);
                return -1;
        }


        int depth = openFile(basePath, dir, 1); //on recupere la profondeur max de cette arborescence
        closedir(dir);

        //on se place sur le token precedent celui que l'on traite
        i = 0;
        currentTok = (*tokList)->first;
        while (i < posArg-1 && currentTok->next != (*tokList)->last){
                currentTok = currentTok->next;
                i++;
        }

        if (basePathEmpty) basePath = "";


        token *newTok;
        token *tmpTok;
        //pour chaque profondeur i inferieure a la profondeur max, on cree le token qui correspond au chemin
        //obtenu en remplacant "**" par i fois "*/"
        for (i = 0; i <= depth; i++) {
                if (strlen(followPath) == 0 && i == 0) continue;

                newTok = malloc(sizeof(token));
                if (newTok == NULL) {
                        if (!basePathEmpty) free(basePath);
                        free(followPath);
                        free(pattern);
                        perror("Echec de l'allocation de memoire a newTok\n");
                        return -1;
                }
                if (type == ARG) {
                        tmpTok = currentTok->next;
                        currentTok->next = newTok;
                        if (tmpTok == NULL)
                                (*tokList)->last = newTok;
                        else
                                tmpTok->precedent = newTok;
                        newTok->precedent = currentTok;
                        newTok->next = tmpTok;
                        newTok->type = ARG;
                } else if (type == CMD) {
                        tmpTok = currentTok->precedent;
                        if (tmpTok == NULL)
                                (*tokList)->first = newTok;
                        else
                                tmpTok->next = newTok;
                        currentTok->precedent = newTok;
                        newTok->precedent = tmpTok;
                        newTok->next = currentTok;
                        newTok->type = CMD;
                }
                newTok->nbEtoileFrom2 = i;
                newTok->nbEtoileNom = 0;
                newTok->currentEtoileNom = 0;
                newTok->name = malloc((2 * i) - 1 + strlen(followPath) + 1);
                if (newTok->name == NULL) {
                        free(newTok);
                        if (!basePathEmpty) free(basePath);
                        free(followPath);
                        free(pattern);
                        perror("Echec de l'allocation de memoire a newTok->name\n");
                        return -1;
                }
                for (int j = 0; j < i; j++) {
                        if (j == i-1){
                                memmove(newTok->name + j * 2, "*", 1);
                        }else {
                                memmove(newTok->name + j * 2, "*/", 2);
                        }
                }
                if (i == 0)
                        memmove(newTok->name, followPath + 1, strlen(followPath));
                else
                        memmove(newTok->name + i * 2 - 1, followPath, strlen(followPath) + 1);
                currentTok = currentTok->next;
                if (!(strcmp(newTok->name, ""))) {
                        tmpTok = currentTok->precedent;
                        freeToken(*tokList, currentTok);
                        currentTok = tmpTok;
                }

        }

        //on supprime le token qui a servi de base
        if (type == CMD && currentTok != NULL)
                freeToken(*tokList, currentTok->precedent);
        else if (currentTok != NULL)
                freeToken(*tokList, currentTok->next);
        *nbArg = *nbArg + depth;
        if ((!strcmp(followPath, "/") || !strcmp(followPath, "")) && basePathEmpty)
                *nbArg = *nbArg -1;
        if (!basePathEmpty) free(basePath);
        free(followPath);
        free(pattern);
        return 0;
}
