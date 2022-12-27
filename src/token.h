#ifndef PROJET_SY5_TOKEN_H

#define PROJET_SY5_TOKEN_H

enum tokenType{
    ARG,
    CMD,
    OPE,
    PIPE,
    REDIRECT
};

enum redirection{
	STDIN,
	STDOUT,
	STDERR,
	STDOUT_TRUNC,
	STDOUT_APPEND,
	STDERR_TRUNC,
	STDERR_APPEND,
	NO_REDIR
};

typedef struct token{
    char *name;
    enum tokenType type;
    enum redirection redir_type; 
    struct token *precedent;
    struct token *next;
} token;

typedef struct tokenList{
    size_t len;
    struct token *first;
    struct token *last;
} tokenList;


struct tokenList *makeTokenList();

void freeToken(struct tokenList *tokList, token *current);

int makeToken(struct tokenList *tokList, const char *name, enum tokenType tokType, enum redirection redir_type);

void clearTokenList(struct tokenList *tokList);

#endif
