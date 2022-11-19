#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int pwdL (char *path);
int pwdP (char *result);

int main (int argc, char *argv[]){
    char *path;
    if (argc==1){
        return pwdL(path);
    }
    //Si on doit faire pwd retourne meme si les argument ne sont pas valide enlever cette ligne et l'autre signalé
    if (argc>2){
        printf("-slash: too many arguments \n");
        return 1;
    }

    if ((argv[1]) && (!strcmp((argv[1]), "-L"))) {

        return pwdL(path);
    }
    if ((argv[1]) && (!strcmp((argv[1]), "-P"))){
        return pwdP(path);
    }
    //Celle-ci
    printf("-slash: invalid option \n");
    return 1;
}

int pwdP (char *result){
    char path[256];
    if (getcwd(path,sizeof (path) )== NULL){
        perror("Something goes wrong with getcwd \n");
        return 1;
    }
    printf("%s \n",path);
    result=&path[0];
    return 0;
}

int pwdL (char *path){
    path= getenv("PWD");
    if (path == NULL){
        perror("Something goes wrong with getenv \n");
        return 1;
    }
    printf("%s \n",path);
    return 0;
}