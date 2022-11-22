#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"

int lexer(char *input, struct tokenList *tokList){
    const char *delimiters = " \t";
    char *tmp = (char *) malloc(strlen(input));
    if (tmp == NULL){
        perror("Echec de l'allocation de memoire a tmp\n");
    }
    strcpy(tmp, input);
    char *tokenStr = strtok(tmp, delimiters);
    while (tokenStr != NULL ) {
        printf ( "%s\n", tokenStr );
        tokenStr = strtok (NULL, delimiters);
    }
    return 1;
}