#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "cd.h"
#include "exit.h"
#include "pwd.h"
#include "token.h"
#include "parser.h"
#include "external.h"
#include "joker.h"
#include "slasherr.h"
#include "redirection.h"

static struct cmdFun tabFun[] = {
        { "cd", exec_cd },
        { "pwd", exec_pwd },
        { "exit", exec_exit },
};
// int parserRedir(struct tokenList **tokList, int len, int *fdin, int *fdout, int *fderr, char ** argv){


// 	int (*fun)(int, int, int, int, char**) = NULL;
//         for (int i = 0; i < sizeof(tabFun)/sizeof(tabFun[0]); i++){
//                 if (!strcmp(tabFun[i].cmdName,(*tokList)->first->name)){
//                         fun = tabFun[i].fun;
//                 }
//         }

// 	ret_val = compute_redirection(fdin,fdout, fderr, tokList);
// 	if(ret_val != 0) return 1;

// 	if(fun == NULL){
// 		return exec_external(*fdin,*fdout, *fderr, len, argv);
// 	}else{
// 		return fun(*fdin, *fdout, *fderr, k, argv);
// 	}
// }

int parserAux(struct tokenList **tokList, struct tokenList **fullTokList, int len, int * fdin, int * fdout, int * fderr, int *pipefds, int n_pipes){

        if ((*tokList)->first == NULL) return 1;
        int firstCmd = ((*tokList)->first == (*fullTokList)->first);
        int lastCmd = ((*tokList)->last == (*fullTokList)->last);

        char **argv = calloc(len,sizeof(char *));
        if (argv == NULL) {
                print_err(NULL, MALLOC_ERR);
                return 1;
        }

        token *current = (*tokList)->first;

	//if(current -> type != REDIRECT)
        argv[0] = current->name;
	int k =1;
        for (int i = 1; i < len; i++){
		/* Ne pas mettre les redirections dans argv */ 
		current = current->next;
		if(current->type == REDIRECT) break;
		argv[i] = current->name; 
		k++;   
        }

        int ret_val;
        current = (*tokList)->first;
        for (int i = 0; i < len; i++){ // Parcours des arguments 

		if (!argv[i]) continue;
		for (int j = 0; j < strlen(argv[i]); j++){ // Parcours d'un argument
                        //printf("DANS LA BOUUUUUUCLE\n");
			if (argv[i][j] == '*'){
				//printf("HERRRRRRRRE\n");
                                if (j+1 < strlen(argv[i]) && argv[i][j+1] == '*')
                                        ret_val = expand_double(argv, tokList, i, &len, current->type);
                                else
                                        ret_val = expand_path(argv, tokList, i, &len, current->type);

                                if (firstCmd) (*fullTokList)->first = (*tokList)->first;
                                if (lastCmd) (*fullTokList)->last = (*tokList)->last;
                                
				if (ret_val == 0){
                                         argv = realloc(argv, len*sizeof (char *));
                                         if (argv == NULL) {
                                                 print_err(NULL, MALLOC_ERR);
                                                 return 1;
                                         }
                                         current = (*tokList)->first;
                                         argv[0] = current->name;

                                         for (int i = 1; i < len; i++){
                                                 current = current->next;
                                                 argv[i] = current->name;
                                         }
                                         current = (*tokList)->first;
                                         i = -1;
                                         break;
				}
				else if (ret_val == -1) {
					len -= 1;
					if (current == (*tokList)->first) return 1;
					freeToken(*tokList, current);

                                         argv = realloc(argv, len*sizeof (char *));
					if (argv == NULL) {
						print_err(NULL, MALLOC_ERR);
						return 1;
					}
					current = (*tokList)->first;
					argv[0] = current->name;

                                         for (int i = 1; i < len; i++){
						current = current->next;
						argv[i] = current->name;
					}
					current = (*tokList)->first;
					i = -1;

					break;
				}
				else return 1;
                        }
                }
		
		if (i != -1) current = current->next;
        }

        int (*fun)(int, int, int, int, char**) = NULL;
        for (int i = 0; i < sizeof(tabFun)/sizeof(tabFun[0]); i++){
                if (!strcmp(tabFun[i].cmdName,(*tokList)->first->name)){
                        fun = tabFun[i].fun;
                }
        }

	ret_val = compute_redirection(fdin,fdout, fderr, tokList);
	if(ret_val != 0) return 1;

	if(fun == NULL){
		return exec_external(*fdin,*fdout, *fderr, len, argv);
	}else{
		return fun(*fdin, *fdout, *fderr, k, argv);
	}
}

/**
Approche pour les redirections : 
	pipeline : stocker le nombre de pipe pour implémenter la pipeline après
 */

int parser(struct tokenList *tokList, char **argCmd, int n_pipes ){ /*PIPES*/


        token *current = tokList->first;
        if (current->type != CMD) return 1;
	token *startCmd = tokList->first;
        token *tmp;
        
	struct tokenList *partial = makeTokenList();
        
	int len = 0;
	int val_ret = 0;
	int j = 0 ;

	int fdin = STDIN_FILENO;
	int fdout = STDOUT_FILENO;
	int fderr = STDERR_FILENO;


	int pipefds [2*n_pipes];

	/* parent creates all needed pipes at the start */
	for(int i = 0; i < n_pipes; i++ ){
		if( pipe(pipefds + i*2) < 0 ){
			print_err(NULL, "pipe error");
			return 1;
		}
	}

        while (current != NULL){
                if (current->type == CMD){
                        len = 1;
                        *startCmd = *current;
                }
                if (current->next == NULL || current->next->type == PIPE) {

			if(n_pipes > 0){
				if (j!=0) {
					/* if not first command */
					if (redirect(pipefds[(j-1)*2], 0) < 0 ) return 1;
					fdin = pipefds[j*2];
				}
				
				if(current->next) {/* if not last command */
				
					if(redirect(pipefds[j*2+1], 1) < 0 ) return 1;
					fdout = pipefds[j*2 + 1];
				}
			}

			tmp = current;
			if(current->next && current->next->type == PIPE){ /*PIPES*/
				current = current->next->next;
			}else{
				current = current->next;
			}
                        while(partial->first != NULL)
                                freeToken(partial, partial->first);
                        
			partial->first = startCmd;
                        partial->last = tmp;
			
			val_ret = parserAux(&partial, &tokList, len, &fdin, &fdout, &fderr, pipefds, n_pipes);
                        if (val_ret){
                                free(partial);
                                return val_ret;
                        }
			j++;
                }
                if (current == NULL){
                        break;
                }
                else current = current->next;
                len++;
        }

	/* parent closes all of its copies at the end */
	for(int i = 0; i < 2 * n_pipes; i++ ){
		close(pipefds[i] );
	}

        free(partial);
        return 0;
}