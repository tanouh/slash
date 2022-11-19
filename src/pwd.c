#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int pwdL (char *path);
int pwdP (char *result);

int main (int argc, char *argv[]){
    printf("ERROR: Blabla \n");
    char *path;
    if (argc==1){
        printf("ERROR: PWD \n");
        return pwdL(path);
    }
    if (argc>2){
        printf("ERROR: Too many parameters \n");
        return 1;
    }

    if ((argv[1]) && (!strcmp((argv[1]), "-L"))) {

        printf("ERROR: L \n");
        return pwdL(path);
    }
    if ((argv[1]) && (!strcmp((argv[1]), "-P"))){
        printf("ERROR: P \n");

        return pwdP(path);
    }
    printf("ERROR: Argument not valid \n");
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