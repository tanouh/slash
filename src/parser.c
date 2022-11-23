#include <stdlib.h>
#include <stdio.h>
#include "cd.h"
#include "exit.h"
#include "pwd.h"
#include "token.h"
#include "parser.h"

static struct cmdFun tab[] = {
        { "cd", exec_cd },
        { "pwd", exec_pwd },
        { "exit", exec_exit },
};

void parser(struct tokenList *tokList, char **argCmd){
        token *current = tokList->first;
        if (current == NULL || current->type != CMD) return ;
        printf("cmd : %s\n", current->name);
        while (current->next != NULL && current->next->type == ARG){
                current = current->next;
                printf("args : %s", current->name);
                //faire une fonction auxiliaire avec deux pointeurs (deb et fin)
                //laisser current dans parser
        }
        printf("\n");
        //appel cmd(strlen(argCmd, argCmd);
}