#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cd.h"
#include "exit.h"
#include "pwd.h"
#include "token.h"
#include "parser.h"
#include "external.h"
#include "slasherr.h"

static struct cmdFun tabFun[] = {
        { "cd", exec_cd },
        { "pwd", exec_pwd },
        { "exit", exec_exit },
};

int parserAux(token *first, token *last, int len){
        if (first == NULL) return 1;
        char **argv = calloc(len,sizeof(char *));
        if (argv == NULL) {
                print_err(NULL, MALLOC_ERR);
                return 1;
        }
        int (*fun)(int, char**) = NULL;
        for (int i = 0; i < sizeof(tabFun)/sizeof(tabFun[0]); i++){
                if (!strcmp(tabFun[i].cmdName,first->name)){
                       fun = tabFun[i].fun;
                }
        }
        int k;
        // if (fun == NULL) return 1;
        // while (first != last && first->next != last){
        //         first = first->next;
        //         argv[k] = first->name;
	// 	k++;
        // }
        // if (len > 0) argv[len-1] = last->name;

        // return fun(len, argv);

	if (fun != NULL){
		k=0;
	}else{
		argv[0] = first->name;
		len +=1;
		k=1;
	}
	while (first != last && first ->next != last){
		first = first->next;
		argv[k] = first->name;
		k++;
	}
	if (len > 0) argv[len-1] = last->name;

	if(fun == NULL){
		return exec_external(len, argv);
	}else{
		return fun(len,argv);
	}
}


int parser(struct tokenList *tokList, char **argCmd){
        token *current = tokList->first;
        if (current->type != CMD) return 1;
        token *startCmd = tokList->first;
        int len = 0;
        while (current != NULL){
                if (current->type == CMD){
                        len = 0;
                        *startCmd = *current;
                }
                if (current->next == NULL || current->next->type == CMD) {
                        if (parserAux(startCmd, current, len)) return 1;
                }
                current = current->next;
                len++;
        }
        return 0;
}