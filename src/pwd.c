#include <stdio.h>
#include <unistd.h>
//#include "conio.h"


int main (int argc, char *argv[]){
    char path[256];
    // Gestion du numero d'arguments
    if (argc>2){
        perror("ERROR: Too many parameters \n");
        return 1;
    }
    if (getcwd(path,sizeof (path) )== NULL){
            perror("Something goes wrong with getcwd \n");
            return 1;
        }
        printf("%s \n",path);
    //getch();
    return 0;
}