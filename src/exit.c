#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"

extern int ret_val;
extern char *prompt;
extern struct tokenList *toklist;

int exec_exit(int argc, char **argv){
        if (argc > 2 || argc == 0){
		free(argv);
		write(stderr,"-slash: exit:  too much arguments \nexit: use: exit [val]",58);
		return 1;
	}
	free(prompt);
	clearTokenList(toklist);
	if(argc == 1){
		free(argv);
		exit(ret_val);
	}
	if (argc == 2 && !(strcmp(argv[1],"0"))){
		free(argv);
		exit(0);
	} else {
		int r = atoi(argv[1]);
		free(argv);
		exit(r);
	}
}
