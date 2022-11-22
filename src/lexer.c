#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "token.h"

int lex(char *input, tokenList *tokList) {
        const char *delimiters = " ";
        char *tmp = malloc(strlen(input) + 1);
        if (tmp == NULL) {
                perror("Echec de l'allocation de memoire a tmp\n");
        }
        strcpy(tmp, input);
        char *tokenStr = strtok(tmp, delimiters);
        char *s = malloc(sizeof(*s) * (strlen(tokenStr) + 1));
        strcpy(s, tokenStr);
        if (tokenStr == NULL) {
                return 1;
        }
        makeToken(tokList, s, CMD);
        tokenStr = strtok(NULL, delimiters);
        while (tokenStr != NULL) {
                printf("%s\n", tokenStr);
                if (!strcmp(tokenStr, "|")) makeToken(tokList, tokenStr, OPE);
                else if (!strcmp(tokenStr, "<") || !strcmp(tokenStr, ">")) makeToken(tokList, tokenStr, REDIRECT);
                else makeToken(tokList, tokenStr, ARG);
                tokenStr = strtok(NULL, delimiters);
        }
        return 1;
}