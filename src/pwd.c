#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main (int argc, char *argv[]){
    char *path;
    if (argc>2){
        perror("ERROR: Too many parameters \n");
        return 1;
    }
    if ((argv[1]!= NULL) && (strcmp(argv[1]), "-P")){
        return pwdP(path);
    }
    return pwdL(path);
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
        perror("Something goes wrong with getcwd \n");
        return 1;
    }
    printf("%s \n",path);
    return 0;
}