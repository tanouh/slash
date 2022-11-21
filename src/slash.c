#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>


#define SIZE_PROMPT 30
#define SIZE_VALRET 3
#define SIZE_COLOR 16

#define GREEN "\001\033[32m\002"
#define RED "\001\033[91m\002"
#define BLUE "\001\033[34m\002"
#define CYAN "\001\033[36m\002"
#define BASIC "\001\033[00m\002"

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

int main() {
    initialice_prompt();
    //char *read = readline(prompt);
    /*
    while (read != NULL){
        free(prompt);
        add_history(read);
    }*/
}

