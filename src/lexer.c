#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int lexer(char *input){
    const char *delimiters = " \t";
    char *tmp;
    strcpy(tmp, input);
    char *tokens = strtok(tmp, delimiters);
    while (tokens != NULL ) {
        printf ( "%s\n", tokens );
        tokens = strtok (NULL, delimiters);
    }
    return 1;
}