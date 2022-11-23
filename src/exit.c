#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "token.h"

extern int ret_val;
extern char *prompt;
extern struct tokenList *toklist;

int exec_exit(int argc, char **argv){
        if (argc > 1 || argc < 0){
		write(STDERR_FILENO,"-slash: exit:  too much arguments \nexit: use: exit [val]",58);
		return 1;
	}
	clearTokenList(toklist);
	if(argc == 0){
		exit(ret_val);
	}
	if (argc == 1 && !(strcmp(argv[0],"0"))){
		exit(0);
	} else {
		int r = atoi(argv[0]);
		exit(r);
	}
}
