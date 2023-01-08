# Architecture du projet :

## Structures de Données:
Nous avons implémenté un lexer, un parser , des tokens et des tokenList  pour la gestion de l’interpréteur.

## Les modules 
**Formatage du prompt (slash.c) :**
Tout d’abord, on a mis en place la configuration du prompt comme elle nous était demandée, en utilisant la variable d'environnement PWD (à travers la fonction getenv()) pour obtenir la référence (logique) du répertoire courant. Puis on a considéré un éventuel tronquage si elle dépassait les 30 caractères ou si l'exécution d’une commande était interrompue par un signal (sa valeur de retour sera égale à 255), dans ce cas, on affiche SIG au lieu de la valeur de retour.


### Lexer :
Découpe la ligne de commande selon les espaces.
Le lexer reconnaît les commandes selon leur position dans la ligne de commande,  ainsi que leurs arguments et  toutes les redirections.
Les tubes anonymes ne sont toutefois pas encore fonctionnels (cf `les redirections` pour plus de détails).
Toutes les données qui ne sont pas catégorisées (commandes/redirections)  sont considérées comme des arguments.

Il crée les tokens correspondants et  les met dans une `tokenList` qui est traitée dans le parser. 


### Parser :
Notre parser est constitué de trois parties : le parser principal, le parser auxiliaire et le parser des redirections.
Le rôle du parser principal est de lire la tokenList et d’en extraire les commandes pour les envoyer dans le parser auxiliaire une par une sous forme de “sous tokenLists”. 
Le parser auxiliaire traite les jokers, et le parser des redirections s’occupe de déterminer si la fonction à appeler est interne ou externe, de traiter les redirections et d’appeler la fonction à exécuter.


### Commandes internes/externes : `pwd.c , cd.c, exit.c, external.c` 
**external.c**
Le module se charge du clonage et des recouvrements nécessaires pour l’exécution des commandes externes. 
Les arguments de la commande sont 'formatés' afin de concorder avec notre choix d’utiliser `execvp`. 

On y traite également la gestion des signaux envoyés aux processus fils.

**les commandes internes**

***cd.c***
Prend un chemin et un indicateur pour savoir s'il faut utiliser des chemins physiques ou non. Si c’est physique, elle essaiera de changer le répertoire de travail courant vers le chemin spécifié en utilisant un chemin absolu, et sinon, elle essaiera de changer le répertoire de travail courant vers le chemin spécifié en utilisant un chemin relatif. Si l'une de ces tentatives échoue, elle essaiera l'autre approche. Si les deux échouent, elle affichera un message d'erreur.
Elle emploie des fonctions auxiliaires pour normaliser les chemins et enlever les parcours redondants (comme .. et .).

***pwd.c*** 
Traite les arguments de la ligne de commande et appelle ensuite soit la fonction pwdL (chemin logique) soit la fonction pwdP (chemin absolu ou physique). La fonction pwdL affiche la valeur de la variable d'environnement PWD (grâce à la fonction getenv) , tandis que la fonction pwdP affiche le chemin physique du répertoire de travail courant (grâce à la fonction realpath)

***exit.c***
Termine directement le programme avec la valeur de retour donnée, ou 0 par défaut. 

### Les redirections : 

`slash` gère toutes les redirections exceptées les redirections par des tubes anonymes. L’approche pour cette dernière fonctionnalité aurait été de stocker toutes les commandes reliées par les pipes `cmd1 | cmd2 | … | cmdn` dans une liste et de les exécuter en une seule fois par clonage et recouvrement successifs à l’aide d’une boucle `for` conditionnée par le nombre de pipes utilisés. 

En cas d’échec d’une redirection, la ligne de commande saisie n’est pas exécutée, et la valeur de retour est 1.


### Jokers (joker.c) : 
Les jokers sont traités dans le parser auxiliaire : les tokens associés à la commande à traiter sont lus un par un et caractère par caractère jusqu’à trouver un des deux jokers, puis la commande associée au joker trouvé est appelée. On lit ensuite à nouveau tous les tokens, et on répète ce processus jusqu’à ne plus trouver de joker.

**Etoile :**
Dans le cas de l’étoile simple on ouvre le répertoire contenant les fichiers à identifier, et pour chacun on vérifie s’il est adapté ou non. S’il l’est, on ajoute un token représentant le chemin avec le nom du fichier. Les tokens créés auront un argument comptant le nombre de fichiers ayant une étoile simple dans leur nom, afin que le parser auxiliaire ne tienne pas compte de ces étoiles simples ci.

**Double étoile :**
Pour la double étoile, on calcule la profondeur de l’arborescence du répertoire courant en ne suivant que les chemins physiques, puis on remplace la double étoile par 0,1, 2, 3, … profondeur étoiles simples consécutives dans profondeur+1 nouveaux tokens. Ainsi le parser auxiliaire va pouvoir traiter ces tokens un par un comme des suites d’étoiles simples.
Les tokens créés auront un argument comptant le nombre d'étoiles simples provenant de la double étoile, afin qu’on ne tienne pas compte des liens symboliques lors de leur traitement.


### Signaux (external.c):
La fonction qui est chargée d'exécuter les processus ( exec_external ) utilise la fonction sigaction pour définir la fonction handler comme gestionnaire de signaux pour les signaux SIGINT, SIGTERM et SIGKILL. Cela signifie que lorsque l'un de ces signaux est reçu par le processus, la fonction handler sera exécutée.
 Lorsqu'un signal est reçu, cette fonction vérifie si le processus en cours est le processus enfant en utilisant la variable globale child_pid. Si c'est le cas, le processus enfant est tué en envoyant le même signal que celui qui a déclenché l’appel à la fonction de gestion de signaux. 
