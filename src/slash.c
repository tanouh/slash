#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "lexer.h"
#include "cd.h"
#include "exit.h"
#include "token.h"
#include "pwd.h"
#include "parser.h"

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

#define SIZE_PROMPT 30
#define SIZE_VALRET 3
#define SIZE_COLOR 16

#define GREEN "\001\033[32m\002"
#define RED "\001\033[91m\002"
#define BLUE "\001\033[34m\002"
#define CYAN "\001\033[36m\002"
#define BASIC "\001\033[00m\002"

int getInput(char *input) {
    char *buffer;
    if ((buffer = fgets("", MAX_ARGS_STRLEN, stdin))) {
        while (*buffer == ' ' || *buffer == '\t') {
            buffer++;
        }
        if (strlen(buffer) != 0) {
            strcpy(input, buffer);
            return 1;
        }
    }
    return 0;
}

static char *initialize_prompt() {
    int valret = 0;
    char *valret_color;
    if (valret) {
        valret_color = GREEN;
    } else {
        valret_color = RED;
    }
    char *string = malloc(( (SIZE_VALRET) +(SIZE_PROMPT) + 3 * SIZE_COLOR + 1) * sizeof(*string));

    if (string == NULL) {
        valret = 1;
        return NULL;
    }
    char *pwd = getenv("PWD");
    if (strlen(pwd) + (SIZE_VALRET + 1) < SIZE_PROMPT) {
        fprintf(stderr, "%s[%d]%s%s%s$ ", valret_color, valret, CYAN, pwd, BASIC);
    } else {
        char *reduction = pwd + strlen(pwd) + 8 - SIZE_PROMPT;
        fprintf(stderr, "%s[%d]%s%s%s%s$ ", valret_color, valret, CYAN, "...", reduction, BASIC);
    }
    return string;
}

int main() {
    initialize_prompt();
    char *input = (char *) malloc(MAX_ARGS_STRLEN);
    if (input == NULL){
        perror("Echec de l'allocation de memoire a input\n");
        return 0;
    }
    struct tokenList *tokList = makeTokenList();

    while(1) {
        //prompt()
        if (getInput(input)) continue;
        lexer(input, tokList);
        freeTokenList(tokList);
    }

    free(input);
    free(tokList);

    return 0;
}
