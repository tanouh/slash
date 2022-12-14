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

//int remakeArgv(char ***argv, token *first, int len){
//        if (*argv == NULL)
//                *argv = calloc(len, len*sizeof(char *));
//        else
//                *argv = realloc(*argv, len*sizeof (char *));
//        if (argv == NULL){
//                print_err(NULL, MALLOC_ERR);
//                return 1;
//        }
//
//        token *current = first;
//        *argv[0] = first->name;
//
//        for (int i = 1; i < len; i++){
//                current = current->next;
//                *argv[i] = current->name;
//        }
//        return 0;
//
//}

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
                                         i--;
                                 }
                                 else if (ret_val == -1) continue;
                                 else return 1;
                        }
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