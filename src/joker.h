#ifndef PROJET_SY5_JOKER_H
#define PROJET_SY5_JOKER_H

#include <dirent.h>

/**
 * Cette fonction identifie le chemin precedant l'etoile et le chemin suivant à
 * partir du token current et les stocke dans deux char * passees en argument
 * @param basePath le char * dans lequel on va stocker le chemin precedent l'etoile
 * @param followPath le char * dans lequel on va stocker le chemin suivant l'etoile
 * @param current le token contenant le chemin a decouper
 * @return 0 en cas de reussite, -1 sinon
 */
int getExtremity(char **basePath, char **followPath, token *current);

/**
 * Cette fonction verifie qu'un fichier de nom filename peut etre insere a la place de l'etoile
 * @param basePath chemin precedent l'etoile
 * @param followPath chemin suivant l'histoire
 * @param fileName nom du fichier qu'on tente d'inserer dans le chemin
 * @param current token contenant le chemin commplet
 * @return 0 si le fichier peut etre insere, -1 s'il ne peut pas l'etre, et 1 en cas de probleme
 */
int verifFile(char *basePath, char *followPath, char *fileName, token *current);

/**
 * Cette fonction va parcourir une arborescence donnee en distinguant lien symbolique et physique
 * si besoin, et en donne la profondeur maximale
 * @param path le chemin qui pointe sur l'arborescence a parcourir
 * @param dir le repertoire courant
 * @param depth la profondeur courante lors de l'appel de openFile
 * @return la profondeur maximale
 */
int openFile(char *path, DIR *dir, int depth);

/**
 * Cette fonction traite * dans un chemin et ajoute a tokList les chemins possibles
 * @param argv le tableau des arguments de la ligne de commande
 * @param tokList la liste des token correspondant a la commande
 * @param posArg la position dans argv du chemin a traiter
 * @param nbArg le nombre d'element de tokList
 * @param type le type du token a traiter
 * @return 0 en cas de succes et -1 sinon
 */
int expand_path(char **argv, struct tokenList **tokList, int posArg, int *nbArg, enum tokenType type);

/**
 * Cette fonction traite ** en prefixe d'un chemin et ajoute a toklist les
 * chemins possibles sous forme de suites d'* de longueurs inferieures a la profondeur
 * maximale de l'arborescence
 * @param argv le tableau des arguments de la ligne de commande
 * @param tokList la liste des token correspondant a la commande
 * @param posArg la position dans argv du chemin a traiter
 * @param nbArg le nombre d'element de tokList
 * @param type le type du token a traiter
 * @return 0 en cas de succes et -1 sinon
 */
int expand_double(char **argv, struct tokenList **tokList, int posArg, int *nbArg, enum tokenType type);

#endif //PROJET_SY5_JOKER_H
