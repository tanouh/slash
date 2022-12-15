#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cd.h"
#include "exit.h"
#include "pwd.h"
#include "token.h"
#include "parser.h"
#include "external.h"
#include "joker.h"
#include "slasherr.h"

static struct cmdFun tabFun[] = {
        { "cd", exec_cd },
        { "pwd", exec_pwd },
        { "exit", exec_exit },
};

int parserAux(struct tokenList **tokList, struct tokenList **fullTokList, int len){

        if ((*tokList)->first == NULL) return 1;
        int firstCmd = ((*tokList)->first == (*fullTokList)->first);
        int lastCmd = ((*tokList)->last == (*fullTokList)->last);

        char **argv = calloc(len,sizeof(char *));
        if (argv == NULL) {
                print_err(NULL, MALLOC_ERR);
                return 1;
        }

        token *current = (*tokList)->first;
        argv[0] = current->name;

        for (int i = 1; i < len; i++){
                current = current->next;
                argv[i] = current->name;
        }

        int ret_val;
        current = (*tokList)->first;
        for (int i = 0; i < len; i++){
                for (int j = 0; j < strlen(argv[i]); j++){
                        if (argv[i][j] == '*'){
                                ret_val = expand_path(argv, tokList, i, &len, current->type);
                                if (firstCmd) (*fullTokList)->first = (*tokList)->first;
                                if (lastCmd) (*fullTokList)->last = (*tokList)->last;
                                if (ret_val == 0){
                                         argv = realloc(argv, len*sizeof (char *));
                                         if (argv == NULL) {
                                                 print_err(NULL, MALLOC_ERR);
                                                 return 1;
                                         }
                                         current = (*tokList)->first;
                                         argv[0] = current->name;

                                         for (int i = 1; i < len; i++){
                                                 current = current->next;
                                                 argv[i] = current->name;
                                         }
                                         current = (*tokList)->first;
                                         i = -1;
                                         break;
                                 }
                                 else if (ret_val == -1) {
                                         len -= 1;
                                         if (current == (*tokList)->first) return 1;
                                         freeToken(*tokList, current);

                                         argv = realloc(argv, len*sizeof (char *));
                                         if (argv == NULL) {
                                                 print_err(NULL, MALLOC_ERR);
                                                 return 1;
                                         }
                                         current = (*tokList)->first;
                                         argv[0] = current->name;

                                         for (int i = 1; i < len; i++){
                                                 if (current == NULL) perror("1111");
                                                 if (argv[i] == NULL) perror("2222");
                                                 if (current->name == NULL) perror("3333");
                                                 current = current->next;
                                                 argv[i] = current->name;
                                         }
                                         current = (*tokList)->first;
                                         i = -1;

                                         break;
                                 }
                                 else return 1;
                        }
                }if (i != -1) current = current->next;
        }

        int (*fun)(int, char**) = NULL;
        for (int i = 0; i < sizeof(tabFun)/sizeof(tabFun[0]); i++){
                for (int j = 0; j < strlen(tabFun[i].cmdName); j++){
                }
                if (!strcmp(tabFun[i].cmdName,(*tokList)->first->name)){
                        fun = tabFun[i].fun;
                }
        }

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
        token *tmp;
        struct tokenList *partial = makeTokenList();
        int len = 0;
	int val_ret = 0;
        while (current != NULL){
                if (current->type == CMD){
                        len = 1;
                        *startCmd = *current;
                }
                if (current->next == NULL || current->next->type == CMD) {
                        tmp = current;
                        current = current->next;
                        while(partial->first != NULL)
                                freeToken(partial, partial->first);
                        partial->first = startCmd;
                        partial->last = tmp;
			val_ret = parserAux(&partial, &tokList, len);
                        if (val_ret){
                                free(partial);
                                return val_ret;
                        }
                }
                if (current == NULL){
                        break;
                }
                else current = current->next;
                len++;
        }
        free(partial);
        return 0;
}