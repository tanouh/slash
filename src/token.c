#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "slasherr.h"

struct tokenList *makeTokenList() {
        struct tokenList *tokList = malloc(sizeof(tokenList));
        if (tokList == NULL) {
		print_err(NULL, MALLOC_ERR);
                return NULL;
        }
        tokList->first = NULL;
        tokList->last = NULL;
        return tokList;
}

void freeToken(struct tokenList *tokList, token *current){
        if (tokList == NULL || current == NULL) return;
        token *last = current->precedent;
        token *next = current->next;
        if (last != NULL){
                last->next = current->next;
        }
        if (next != NULL){
                next->precedent = current->precedent;
        }
        if (current == tokList->first)
                tokList->first = tokList->first->next;
        if (current == tokList->last)
                tokList->last = tokList->last->precedent;
        free(current->name);
        free(current);
        return;
}

int makeToken(struct tokenList *tokList, const char *name, enum tokenType tokType) {
        token *tok = malloc(sizeof(token));
        if (tok == NULL) {
                print_err(NULL, MALLOC_ERR);
                return 0;
        }
        tok->name = malloc((1 + strlen(name)) * sizeof(char));
        if (tok->name == NULL) {
                print_err(NULL, MALLOC_ERR);
                free(tok);
                return 0;
        }
        strcpy(tok->name, name);
        tok->type = tokType;
        tok->precedent = tokList->last;
        tok->next = NULL;
        if (tokList->first == NULL) {
                tokList->first = tok;
                tokList->last = tok;
        } else {
                tokList->last->next = tok;
        }
        tokList->last = tok;
        return 1;
}

//TODO: rendre la liste générique
//ajouter void *, sizeof (elt), fonction utilitaire pour nettoyer facilement, deplacer le pointeur de k-fois la
//de l'element
void clearTokenList(struct tokenList *tokList) {
        if (tokList == NULL) return;
	token * tmp = tokList->first;
        while (tokList->first != NULL) {
                tmp = tokList->first->next;
                if (tokList->first->name != NULL) free(tokList->first->name);
                free(tokList->first);
                tokList->first = tmp;
        }
}
