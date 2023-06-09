#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "token.h"
#include "slasherr.h"

extern int n_cmds;
extern int n_pipes;
char *lastTokStr;


int lex_one(char *value, tokenList *tokList) /*PIPES*/
{
        enum tokenType type = REDIRECT;
        enum redirection redir_type = NO_REDIR;
        switch (value[0]) {
                case '<':
                        redir_type = STDIN;
                        break;
                case '>':
                        redir_type = STDOUT;
                        break;
                case '|':
                        type = PIPE;
                        n_pipes++;
                        break;
                default :
                        if (!strcmp(lastTokStr, "|"))
                                type = CMD;
                        else
                                type = ARG;
                        break;
        }
        return makeToken(tokList, value, type, redir_type);
}

int lex_two(char *value, tokenList *tokList) {
        enum tokenType type = REDIRECT;
        enum redirection redir_type = NO_REDIR;
        switch (value[0]) {
                case '>':
                        switch (value[1]) {
                                case '|':
                                        redir_type = STDOUT_TRUNC;
                                        break;
                                case '>':
                                        redir_type = STDOUT_APPEND;
                                        break;
                                default :
                                        type = ARG;
                                        break;
                        }
                        break;
                case '2':
                        if (value[1] == '>') {
                                type = REDIRECT;
                                redir_type = STDERR;
                        } else {
                                type = ARG;
                        }
                        break;
                        // return makeToken(tokList, value, type,redir_type);
                default:
                        type = ARG;
                        break;

        }
        return makeToken(tokList, value, type, redir_type);
}

int lex_three(char *value, tokenList *tokList) {

        enum tokenType type = REDIRECT;
        enum redirection redir_type = NO_REDIR;
        if (value[0] == '2') {
                if (value[1] == '>') {
                        switch (value[2]) {
                                case '>':
                                        redir_type = STDERR_APPEND;
                                        break;
                                case '|':
                                        redir_type = STDERR_TRUNC;
                                        break;
                                default :
                                        type = ARG;
                                        break;

                        }
                } else {
                        type = ARG;
                }
        } else {
                type = ARG;
        }
        return makeToken(tokList, value, type, redir_type);
}

struct tokenList *lex(char *input, tokenList *tokList) {
        int val_ret = 0;
        const char *delimiters = " ";
        char *tmp = malloc(strlen(input) + 1);

        if (tmp == NULL) {
                print_err(NULL, MALLOC_ERR);
                return NULL;
        }

        strcpy(tmp, input);
        char *tokenStr = strtok(tmp, delimiters);

        if (tokenStr == NULL) {
                return NULL;
        }

        makeToken(tokList, tokenStr, CMD, NO_REDIR);
        lastTokStr = tokenStr;
        tokenStr = strtok(NULL, delimiters);

	while (tokenStr != NULL )
	{
		switch(strlen(tokenStr)){
			case 1:
				val_ret = lex_one(tokenStr,tokList);
				break;
			case 2:
				val_ret = lex_two(tokenStr,tokList);
				break;
			case 3:
				val_ret = lex_three(tokenStr,tokList);
				break;
			default:
				if(!strcmp(lastTokStr, "|")){
					val_ret = makeToken(tokList,tokenStr,CMD, NO_REDIR);
					n_cmds ++;
				}else{
					val_ret = makeToken(tokList,tokenStr,ARG, NO_REDIR);
				}
					break;
		}
		if(val_ret == 0 ){
                        free(tmp);
			return NULL;
		}
		lastTokStr = tokenStr;
		tokenStr = strtok (NULL, delimiters);
	}
	free(tmp);
	return tokList;
}
