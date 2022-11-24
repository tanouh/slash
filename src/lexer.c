#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "token.h"


/*
TODO : Pour le jalon 2 : tester que l'élément avant n'est pas un | pcq sinon le token serait une commande
*/

int lex_one(char * value, tokenList * tokList)
{
	enum tokenType type = ARG;
	switch(value[0]){
		case '<':
		case '>':
			type = REDIRECT;
			break;
		case '|':
			type = PIPE;
			break;
			
		default : 
			type = ARG;
	}
	return makeToken(tokList,value,type);
}

int lex_two(char * value, tokenList * tokList)
{	
	enum tokenType type = ARG;
	switch(value[0]){
		case '>':
			switch (value[1]){
				case '|': 
				case '>': //append 
					type = REDIRECT;
					break;
				default : 
					type = ARG; 
			}
			break;
		case '2':
			type = (value[1] == '>')? ARG : REDIRECT;
			return makeToken(tokList, value, type);
		default: 
			type = ARG;

	}
	return makeToken(tokList,value,type);	
}

int lex_three(char * value, tokenList * tokList)
{
	enum tokenType type = ARG;
	if(value[1]=='2'){
		switch(value[2]){
			case '>':
			case '|': 
				type = REDIRECT;
				break;
			default : 
				type = ARG;
				
		}
	}
	return makeToken(tokList,value,type);
}

struct tokenList *lex (char *input, tokenList *tokList){
	int val_ret = 0;
	const char *delimiters = " ";
	char *tmp = malloc(strlen(input) +1);

	if (tmp == NULL)
	{
		perror("Echec de l'allocation de memoire a tmp\n");
		return NULL;
	}
	
	strcpy(tmp, input);
	char *tokenStr = strtok(tmp, delimiters);
	
	if (tokenStr == NULL) 
	{
		return NULL;
	}
	
	makeToken(tokList, tokenStr, CMD);
	tokenStr = strtok (NULL, delimiters);
	
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
				val_ret = makeToken(tokList,tokenStr,ARG);
				break;
		}
		if(val_ret == 0 ){
			return NULL;
		}
		tokenStr = strtok (NULL, delimiters);
	}	
	return tokList;
}
