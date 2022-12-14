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

int parserAux(token *first, token *last, int len){

        if (first == NULL) return 1;

        char **argv = calloc(len,sizeof(char *));
        if (argv == NULL) {
                print_err(NULL, MALLOC_ERR);
                return 1;
        }

//        char *fun_Name = first->name;
        int (*fun)(int, char**) = NULL;
        for (int i = 0; i < sizeof(tabFun)/sizeof(tabFun[0]); i++){
                for (int j = 0; j < strlen(tabFun[i].cmdName); j++){
//                        if (tabFun[i].cmdName[j] == '*') fun_Name = expand_cmd();
                }
                if (!strcmp(tabFun[i].cmdName,first->name)){
                        fun = tabFun[i].fun;
                }
        }

        argv[0] = first->name;

        token *current = first;
        for (int i = 1; i < len; i++){
                current = current->next;
                argv[i] = current->name;
        }

        int ret_val;
        current = first;
        token *before;
        token *next;
        for (int i = 0; i < len; i++){
                for (int j = 0; j < strlen(argv[i]); j++){
                        if (argv[i][j] == '*'){
                                 ret_val = expand_path(argv, &first, &last, i, &len);
                                 if (ret_val == 0){
                                         argv = realloc(argv, len*sizeof (char *));
                                         if (argv == NULL) {
                                                 print_err(NULL, MALLOC_ERR);
                                                 return 1;
                                         }
                                         current = first;
                                         argv[0] = first->name;

                                         for (int i = 1; i < len; i++){
                                                 current = current->next;
                                                 argv[i] = current->name;
                                         }
                                         current = first;
                                         i = -1;
                                         break;
                                 }
                                 else if (ret_val == -1) {
                                         len -= 1;
                                         if (last->next == NULL && (current == last || current->next ==  last)){
                                                 if (current == last){
                                                         last = current->precedent;
                                                         last->next = NULL;
                                                         free(current->name);
                                                         free(current);
                                                         current = last;
                                                 }
                                                 if (current->next ==  last){
                                                         current->precedent->next = last;
                                                         last->precedent = current->precedent;
                                                         free(current->name);
                                                         free(current);
                                                         current = last;
                                                 }
                                         }else {
                                                 if (current == first) return 1;
                                                 before = current->precedent;
                                                 next = current->next;
                                                 if (before != NULL)
                                                         before->next = next;
                                                 if (next != NULL)
                                                         next->precedent = before;
                                                 free(current->name);
                                                 free(current);
                                         }

                                         argv = realloc(argv, len*sizeof (char *));
                                         if (argv == NULL) {
                                                 print_err(NULL, MALLOC_ERR);
                                                 return 1;
                                         }
                                         current = first;
                                         argv[0] = first->name;

                                         for (int i = 1; i < len; i++){
                                                 current = current->next;
                                                 argv[i] = current->name;
                                         }
                                         current = first;
                                         i = -1;

                                         break;
                                 }
                                 else return 1;
                        }
                }if (i != -1) current = current->next;
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
        int len = 0;
	int val_ret = 0;
        while (current != NULL){

                if (current->type == CMD){
                        len = 1;
                        *startCmd = *current;
                }
                if (current->next == NULL || current->next->type == CMD) {
			val_ret = parserAux(startCmd, current, len);
                        if (val_ret) return val_ret;
                }
                current = current->next;
                len++;
        }
        return 0;
}