#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "external.h"
#include "slasherr.h"

char **formate_args(int argc, char **argv)
{
	char **arg_list = malloc((argc + 1) * sizeof(char *));
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
		return 1;
	}
	char **args_list = formate_args(argc, argv);
	pid_t proc = fork();
	int stat = 0;
	int retval = 0;
	switch (proc)
	{
	case -1:
		return 1;
		break;
	case 0:
		dup2(STDOUT_FILENO, STDOUT_FILENO);
		int status = execvp(argv[0], args_list);
		if (status == -1)
		{
			print_err(argv[0], "command not found");
			return 1;
		}
	default:
		waitpid(proc, &stat, 0);
		if (WIFEXITED(stat))
		{
			retval = WEXITSTATUS(stat);
		}
		wait(NULL);
		free(args_list);
	}
	return retval;
}
