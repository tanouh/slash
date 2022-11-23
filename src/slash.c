#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "Commands/exit.h"
#include "Commands/cd.h"
#include "Commands/exit.h"
#include "Commands/pwd.h"


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

int ret_val;
char * prompt;
struct tokenList *toklist;

static char *initialize_prompt(int valret) {
        char *valret_color;
        if (valret) {
                valret_color = GREEN;
        } else {
                valret_color = RED;
        }
        char *string = malloc(((SIZE_VALRET) + (SIZE_PROMPT) + 3 * SIZE_COLOR + 1) * sizeof(*string));

        if (string == NULL) {
                valret = 1;
                return NULL;
        }
        char *pwd = getenv("PWD");
        if (strlen(pwd) + (SIZE_VALRET + 1) < SIZE_PROMPT) {
                sprintf(string, "%s[%d]%s%s%s$ ", valret_color, valret, CYAN, pwd, BASIC);
        } else {
                char *reduction = pwd + strlen(pwd) + 8 - SIZE_PROMPT;
                sprintf(string, "%s[%d]%s%s%s%s$ ", valret_color, valret, CYAN, "...", reduction, BASIC);
        }
        return string;
}



int main() {
	ret_val = 0;
        prompt = initialize_prompt(ret_val);
	toklist = makeTokenList();
        rl_outstream = stderr;
        char *buffer;
	
        while ((buffer = readline(prompt)) != NULL) {

                add_history(buffer);
                free(prompt);
                toklist = lex(buffer, toklist);
		ret_val = 0; // valeur renvoyée par le parser
                prompt = initialize_prompt(ret_val);
		clearTokenList(toklist);
		fprintf(stderr,"%p\n",(void *) toklist->first);
        }
        rl_clear_history();

        free(prompt);
        free(toklist);

        return 0;
}
