#ifndef PROJET_SY5_EXTERNAL_H
#define PROJET_SY5_EXTERNAL_H

#include "token.h"
/**
 * Modifie l'array des arguments de sorte que le dernier élément soit NULL 
 */
char **formate_args(int argc, char **argv);

/**
 * Exécute les commandes externes
 */
int exec_external(int *fdin, int *fdout,int *fderr, int argc, char **argv);

#endif
