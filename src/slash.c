#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "lexer.h"
#include "cd.h"
#include "exit.h"
#include "token.h"
#include "pwd.h"
#include "parser.h"
#include "slash.h"
#include "slasherr.h"

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
char *lastWd; // last working directory

char *initialize_prompt(int valret) {
        char *valret_color;
        if (valret == 0) {
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
        rl_outstream = stderr;
	
	ret_val = 0;
        prompt = initialize_prompt(ret_val);
	toklist = makeTokenList();
	lastWd = malloc(MAX_ARGS_STRLEN); 
	if(lastWd == NULL){
		print_err(NULL, MALLOC_ERR);
		exit(1);
	}
        //lastWd = getenv("PWD");
	memset(lastWd, 0x0, MAX_ARGS_STRLEN);
	memmove(lastWd, getenv("PWD"), strlen(getenv("PWD")));
	
	char *buffer;
        char **argCmd;

        while ((buffer = readline(prompt)) != NULL) {
                if (!strcmp(buffer, "")) continue;
                add_history(buffer);
                free(prompt);
		toklist = lex(buffer, toklist);
		free(buffer);
                argCmd = malloc((toklist->len+1)*sizeof(char *));
                if (argCmd == NULL){
                        print_err(NULL, MALLOC_ERR);
                        clearTokenList(toklist);
                        break;
                }
                ret_val = parser(toklist, argCmd);
                free(argCmd);
                clearTokenList(toklist);
                prompt = initialize_prompt(ret_val);
        }
        rl_clear_history();
        free(prompt);
        free(toklist);
	free(lastWd);

	write(STDERR_FILENO,"\n-slash terminate\n",20);
	exit(ret_val);
}
