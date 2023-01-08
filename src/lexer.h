#ifndef PROJET_SY5_LEXER_H
#define PROJET_SY5_LEXER_H

#include "token.h"
int lex_one(char *input, struct tokenList *tokList); /*PIPES*/
int lex_two(char *input, struct tokenList *tokList);
int lex_three(char *input, struct tokenList *tokList);
struct tokenList *lex(char *input, struct tokenList *tokList); /*PIPES*/

#endif //PROJET_SY5_LEXER_H
