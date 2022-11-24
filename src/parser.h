#ifndef PROJET_SY5_PARSER_H
#define PROJET_SY5_PARSER_H


typedef struct cmdFun{
    char *cmdName;
    int (*fun)(int, char**);
}cmdFun;

int parserAux(token *first, token *last, int len);

int parser(struct tokenList *tokList, char **argCmd);

#endif //PROJET_SY5_PARSER_H