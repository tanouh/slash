#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>


#include "external.h"

char ** formate_args(int argc, char ** argv){
	char ** arg_list = malloc((argc + 1)*  sizeof(char *));
	for (int i = 0 ; i < argc ; i++){
		arg_list[i]=argv[i];
	}
	arg_list[argc] = NULL;
	return arg_list;
}

int exec_external(int argc, char ** argv){
	if (argc < 0){
		return 1;
	}
	char ** args_list = formate_args(argc,argv);
	pid_t proc = fork();

	switch(proc){
		case -1:
			return 1;
		case 0:
			dup2(STDERR_FILENO,STDOUT_FILENO);
			int status = execvp(argv[0], args_list);
			if(status == -1) {

				dprintf(STDERR_FILENO, "%s: command not found\n", argv[0]);	

			}
			break;
		default:
			wait(NULL);
			free(args_list);

	} 
	return 0;
}
