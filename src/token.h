#ifndef PROJET_SY5_TOKEN_H

#define PROJET_SY5_TOKEN_H

/**
 * les differents types de token
 */
enum tokenType {
    ARG,
    CMD,
    PIPE,
    REDIRECT
};

/**
 * les differents types de redirection
 */
enum redirection {
    STDIN,
    STDOUT,
    STDERR,
    STDOUT_TRUNC,
    STDOUT_APPEND,
    STDERR_TRUNC,
    STDERR_APPEND,
    NO_REDIR
};


typedef struct token {
    char *name; //valeur de l'argument
    enum tokenType type; //type de token
    enum redirection redir_type; //type de redirection
    struct token *precedent;
    struct token *next;
    int nbEtoileFrom2; //nombre d'etoiles ajoutees & partir de **
    int nbEtoileNom; //nb d'elements de name ce name contenant une * dans leur nom
    int currentEtoileNom; //bool determinant au moment de l'incrementation de nbEtoileNom si
    // nbEtoileNom a deja ete incremente dans l'iteration courante
} token;

typedef struct tokenList {
    size_t len; //longueur de la tokenList
    struct token *first; //premier element
    struct token *last; //dernier element
} tokenList; //liste doublement chainee de tokens

/**
 * Cree une tokenList vide
 * @return la tokenList creee
 */
struct tokenList *makeTokenList();

/**
 * Supprime un token et le retire de la tokList
 * @param tokList la tokenList dont il faut supprimer un token
 * @param current le token a supprimer
 */
void freeToken(struct tokenList *tokList, token *current);

/**
 * Cree un token et l'ajoute a la fin de la tokenList
 * @param tokList
 * @param name
 * @param tokType
 * @param redir_type
 * @return 1 en cas de succes et 0 sinon
 */
int makeToken(struct tokenList *tokList, const char *name, enum tokenType tokType, enum redirection redir_type);

/**
 * Vide une tokenList
 * @param tokList la token list a vider
 */
void clearTokenList(struct tokenList *tokList);

#endif
