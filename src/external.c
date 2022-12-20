#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "external.h"
#include "slasherr.h"

char **formate_args(int argc, char **argv)
{
	char **arg_list = malloc((argc + 1) * sizeof(char *));
	if (arg_list == NULL){
		print_err(NULL, MALLOC_ERR);
		return NULL;
	}
	for (int i = 0; i < argc; i++)
	{
		arg_list[i] = argv[i];
	}
	arg_list[argc] = NULL;
	return arg_list;
}

int exec_external(int argc, char **argv)
{
	if (argc < 0)
	{
		free(argv);
		return 1;
	}
	char **args_list = formate_args(argc, argv);
	if (args_list == NULL) goto error;
	pid_t proc = fork();
	int stat = 0;
	int retval = 0;
	switch (proc)
	{
	case -1:
		goto error;
	case 0:
		dup2(STDOUT_FILENO, STDOUT_FILENO);
		int status = execvp(argv[0], args_list);
		if (status == -1)
		{
			print_err(argv[0], "command not found");
			goto error;
		}
	default:
		waitpid(proc, &stat, 0);
		if (WIFEXITED(stat))
		{
			retval = WEXITSTATUS(stat);
		} else if (WIFSIGNALED(stat)){
            retval = 255;
        } else if (WIFSTOPPED(stat)){
            retval = 255;
        }
		wait(NULL);
		free(args_list);
	}
	free(argv);
	return retval;

	error:
		free(args_list);
		free(argv);
		exit(errno);
}
