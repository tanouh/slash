#ifndef PROJET_SY5_TOKEN_H
#define PROJET_SY5_TOKEN_H

enum tokenType{
    ARG,
    CMD,
    OPE,
    REDIRECT
};

typedef struct token{
    char *name;
    enum tokenType type;
    struct token *precedent;
    struct token *next;
} token;

typedef struct tokenList{
    size_t len;
    struct token *first;
    struct token *last;
} tokenList;


struct tokenList *makeTokenList();

int makeToken(struct tokenList *tokList, const char *name, enum tokenType tokType);

void clearTokenList(struct tokenList *tokList);

#endif
