#ifndef PROJET_SY5_PARSER_H
#define PROJET_SY5_PARSER_H


typedef struct cmdFun{
    char *cmdName;
    int (*fun)(int, int, int, int, char**);
}cmdFun;

int parserAux(struct tokenList **tokList, struct tokenList **fullTokList, int len, int * fdin, int * fdout, int * fderr);

int parser(struct tokenList *tokList, char **argCmd) ;

#endif //PROJET_SY5_PARSER_H