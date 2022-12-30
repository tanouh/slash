#ifndef PROJET_SY5_REDIRECTION_H
#define PROJET_SY5_REDIRECTION_H

#include "token.h"

int compute_redirection (int *fdin, int * fdout, int *fderr, struct tokenList ** tokList);
int redirect(int oldfd, int newfd);


#endif