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
        if (valret == 255){
            if (strlen(pwd) + (SIZE_VALRET + 1) < SIZE_PROMPT) {
                sprintf(string, "%s[%s]%s%s%s$ ", valret_color, "SIG", CYAN, pwd, BASIC);
            } else {
                char *reduction = pwd + strlen(pwd) + 8 - SIZE_PROMPT;
                sprintf(string, "%s[%s]%s%s%s%s$ ", valret_color, "SIG", CYAN, "...", reduction, BASIC);
            }
            return string;
        }
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
	memset(lastWd, 0x0, MAX_ARGS_STRLEN);
	memmove(lastWd, getenv("PWD"), strlen(getenv("PWD")));
	
	char *buffer;
        char **argCmd;

        int lenTokList;
        token *current;
        while ((buffer = readline(prompt)) != NULL) {
                clearTokenList(toklist);
                toklist->first = NULL;
                toklist->last = NULL;
                lenTokList = 0;

                if (!strcmp(buffer, "")) continue;
                add_history(buffer);
                free(prompt);
		toklist = lex(buffer, toklist);
		free(buffer);

                current = toklist->first;
                while(current != NULL){
                        lenTokList++;
                        current = current->next;
                }
                argCmd = malloc((lenTokList+1)*sizeof(char *));
                if (argCmd == NULL){
                        print_err(NULL, MALLOC_ERR);
                        break;
                }
                ret_val = parser(toklist, argCmd);
                free(argCmd);
                prompt = initialize_prompt(ret_val);
                current = NULL;
        }
        free(buffer);
        rl_clear_history(); // à voir si ça ne pose pas problème
        free(prompt);
        if (toklist->first != NULL)
                clearTokenList(toklist);
        free(toklist);
	free(lastWd);

	write(STDERR_FILENO,"\n",1);
	exit(ret_val);
}
