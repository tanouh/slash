#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "cd.h"
#include "exit.h"
#include "pwd.h"
#include "token.h"
#include "parser.h"
#include "external.h"
#include "joker.h"
#include "slasherr.h"
#include "redirection.h"

extern int n_pipes;
extern int n_cmds;

int fdpipe[2];


static struct cmdFun tabFun[] = {
        {"cd",   exec_cd},
        {"pwd",  exec_pwd},
        {"exit", exec_exit},
};

command * makeCommand(char ** argv, int len, int *fdin, int *fdout, int *fderr){
	command * cmd = malloc(sizeof(command));
	if(cmd == NULL) {
		print_err("parser", MALLOC_ERR);
		return NULL;
	}
	cmd->argv = argv;
	cmd->len= len;
	cmd->fdin = fdin;
	cmd->fdout = fdout;
	cmd->fderr= fderr;
	cmd->next = NULL;
	return cmd;
}

int execute_commands (command * firstCmd){
	int ret_val = 0 ;
	command * current_cmd = firstCmd;
	char * cmdName = NULL; 
	int (*fun)(int*, int*, int*, int, char**) = NULL;

	int i = 0; 

	while(current_cmd != NULL){
		cmdName = (current_cmd->argv)[0];
		
		/*Commandes internes*/
		for (i = 0; i < sizeof(tabFun)/sizeof(tabFun[0]); i++){
			if (!strcmp(tabFun[i].cmdName,cmdName)){
				fun = tabFun[i].fun;
			}
		}

		if(n_pipes > 0){
			if(pipe(fdpipe) == -1){
				print_err(cmdName, "Pipe error");
				return 1;
			}

			ret_val = exec_external(current_cmd->fdin,current_cmd->fdout, current_cmd->fderr, current_cmd->len, current_cmd->argv);

		}else{
			if(fun == NULL){
				return exec_external(current_cmd->fdin,current_cmd->fdout, current_cmd->fderr, current_cmd->len, current_cmd->argv);
			}else{
				return fun(current_cmd->fdin,current_cmd->fdout, current_cmd->fderr, current_cmd->len, current_cmd->argv);
			}
		}
		current_cmd = current_cmd -> next;
	}
	return ret_val;

}


command * parserRedir(struct tokenList **tokList, int len, int *fdin, int *fdout, int *fderr, char ** argv){

	token *current = (*tokList)->first;
	//if(current -> type != REDIRECT)
        argv[0] = current->name;
        int k = 1;
        for (int i = 1; i < len; i++) {
                /* Ne pas mettre les redirections dans argv */
                current = current->next;
                if (current->type == REDIRECT) break;
                argv[i] = current->name;
                k++;
        }
	
	int ret_val = compute_redirection(fdin,fdout, fderr, tokList);
	
	if(ret_val != 0) return NULL;

	command * cmd = makeCommand(argv,k,fdin,fdout,fderr);

	return cmd;
}

command * parserAux(struct tokenList **tokList, struct tokenList **fullTokList, int len, int * fdin, int * fdout, int * fderr, char** argv){
	if(!argv) return NULL;
        if ((*tokList)->first == NULL) return NULL;
        int firstCmd = ((*tokList)->first == (*fullTokList)->first);
        int lastCmd = ((*tokList)->last == (*fullTokList)->last);

        token *current = (*tokList)->first;

        argv[0] = current->name;
        for (int i = 1; i < len; i++) {
                current = current->next;
                argv[i] = current->name;
        }

        int ret_val;
        int cpt;
        current = (*tokList)->first;
        for (int i = 0; i < len; i++) { // Parcours des arguments
                cpt = 0;
                if (!argv[i]) continue;
                for (int j = 0; j < strlen(argv[i]); j++) { // Parcours d'un argument
                        if (argv[i][j] == '*') {

                                if (cpt++ < current->nbEtoileNom) continue;
                                if (j + 1 < strlen(argv[i]) && argv[i][j + 1] == '*')
                                        ret_val = expand_double(argv, tokList, i, &len, current->type);
                                else
                                        ret_val = expand_path(argv, tokList, i, &len, current->type);

                                if (firstCmd) (*fullTokList)->first = (*tokList)->first;
                                if (lastCmd) (*fullTokList)->last = (*tokList)->last;

				if (ret_val == 0){
                                         argv = realloc(argv, len*sizeof (char *));
                                         if (argv == NULL) {
                                                 print_err(NULL, MALLOC_ERR);
                                                 return NULL;
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
					if (current == (*tokList)->first) return NULL;
					freeToken(*tokList, current);

                                        argv = realloc(argv, len*sizeof (char *));
					if (argv == NULL) {
						print_err(NULL, MALLOC_ERR);
						return NULL;
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
				else return NULL;
                        }
                }

                if (i != -1) current = current->next;
        }
	return parserRedir(tokList, len,fdin, fdout,fderr,argv);
}

int parser(struct tokenList *tokList, char **argCmd){ 
	// if (n_pipes > n_cmds) {
	// 	print_err("pipe", "erreur pipe");
	// 	exit(2);
	// }

        token *current = tokList->first;
        if (current->type != CMD) {
		return 1;
	}
	token *startCmd = tokList->first;
        token *tmp;
        
	struct tokenList *partial = makeTokenList();

        command * current_cmd = NULL;
	command * last_cmd = NULL;
	command * first_cmd = NULL;
	int len = 0;
	int val_ret = 0;
	
	int cmd_i = 0;
	int fdin = STDIN_FILENO;
	int fdout = STDOUT_FILENO;
	int fderr = STDERR_FILENO;


        while (current != NULL){
                if (current->type == CMD){
                        len = 1;
                        *startCmd = *current;
                }
                if (current->next == NULL || current->next->type == PIPE) {

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

			char **argv = calloc(len,sizeof(char *));
			if (argv == NULL) {
				free(current_cmd);
				print_err(NULL, MALLOC_ERR);
				return 1;
			}

			current_cmd = parserAux(&partial, &tokList, len, &fdin, &fdout, &fderr, argv);
			if(current_cmd){
				if(cmd_i == 0) first_cmd = current_cmd; //première commande à exécuter
				current_cmd -> next = last_cmd;
			}else{
				if(last_cmd) free(last_cmd);
				if(first_cmd) free(first_cmd);
				free(partial);
				return 1;
			}		
			cmd_i++;

                }
                if (current == NULL ){
                        break;
                }
                else {
			current = current->next;
			if(current_cmd){
				last_cmd= current_cmd;
				current_cmd = current_cmd -> next;
			}
		}
                len++;
        }

	val_ret = execute_commands(first_cmd);

        free(partial);
	free(current_cmd);
        return val_ret;
}