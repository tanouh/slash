#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> /* pid_t */
#include <fcntl.h>
#include <unistd.h>

#include "redirection.h"
#include "token.h"
#include "slasherr.h"

int compute_redirection(int *fdin, int * fdout, int *fderr, struct tokenList ** tokList){
	token * current = (*tokList)->first;
	while(current != NULL){
		if (current->type == REDIRECT){
			if(current->next){
				token * file = current -> next;
				
				if(file -> type == ARG){
					switch (current->redir_type) {
						case STDIN : 
							*fdin = open(file->name , O_RDONLY);
							if(*fdin < 0){
								goto error;
							}
							break;
						case STDOUT :
							*fdout = open(file->name, O_CREAT | O_EXCL | O_WRONLY, 0664);
							if(*fdout < 0){
								goto error;
							}
							break;
						case STDERR : 
							*fderr = open(file->name, O_CREAT | O_EXCL | O_WRONLY,0664);
							if(*fderr < 0){
								goto error;
							}
							break;
						case STDOUT_TRUNC:
							*fdout= open(file->name,O_CREAT | O_TRUNC | O_WRONLY,0664);
							if(*fdout < 0){
								goto error;
							}
							break;
						case STDOUT_APPEND:
							*fdout= open(file->name, O_CREAT | O_APPEND | O_WRONLY,0664);
							if(*fdout < 0){
								goto error;
							}
							break;
						case STDERR_TRUNC:
							*fderr = open (file->name, O_CREAT | O_TRUNC | O_WRONLY,0664);
							if(*fderr < 0){
								goto error;
							}
							break;
						case STDERR_APPEND:
							*fderr= open (file -> name, O_CREAT | O_APPEND | O_WRONLY,0664);
							if(*fderr < 0){
								goto error;
							}
						default : break; 
					}

				}	
			}	
		}
		current = current -> next; 
	}
	return 0; 

	error : 
		print_err_fd(*fderr, NULL, "Error while opening the file.");
		return 1;
}

int redirect(int oldfd, int newfd){
	if(oldfd != newfd){
		if(dup2(oldfd,newfd) != -1){
			close(oldfd); /* successfully redirected*/
			return 0;
		}else{
			print_err(NULL, "dup2 error");
			return 1;
		}
	}
	return 0;
}
