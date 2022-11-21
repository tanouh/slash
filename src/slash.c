#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <lexer.c>
#include <exit.c>

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

int getInput(char *input){
    char *buffer;
    if ((buffer = fgets(" ", MAX_ARGS_STRLEN, stdin))){
        while (*buffer == ' ' || *buffer == '\t'){
            buffer++;
        }
        if (strlen(buffer) != 0) {
            strcpy(input, buffer);
            return 1;
        }
    }
    return 0;
}

int main(){
    char *input = (char *) malloc(MAX_ARGS_STRLEN);
    while(1){
        //prompt()
        if(getInput(input)) continue;
        lexer(input);

    free(input);
    return 0;
    }
}
