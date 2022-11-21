#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <lexer.c>
#include <exit.c>

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

static char *initialice_prompt() {
    int valret = 0;
    printf("hhh");
    char *prompt = malloc((SIZE_PROMPT + 4 * strlen(GREEN) + 1) * sizeof(prompt));
    if (prompt = NULL) {
        valret = 1;
        return NULL;
    }
    char *valret_color = malloc( SIZE_COLOR* sizeof(valret_color));
    if (valret) {
        valret_color = RED;
    } else {
        valret_color = GREEN;
    }
    char *pwd = getenv("PWD");
    if (strlen(pwd) + 8 + (SIZE_VALRET + 1) < SIZE_PROMPT) {
        sprintf(prompt, "[%s%d]%s%s%s$", valret_color, valret, CYAN, pwd, BASIC);
    } else {
        char *reduction = pwd + strlen(pwd) + 8 - SIZE_PROMPT;
        sprintf(prompt, "[%s%d]%s%s%s%s$", valret_color, valret, CYAN, "...", reduction, BASIC);
    }
    return prompt;
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
