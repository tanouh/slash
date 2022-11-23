#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"

extern int ret_val;
extern char *prompt;
extern struct tokenList *toklist;

int compute_exit(int argc, char **argv){
        if (argc > 2 || argc == 0){
		write(stderr,"-slash: exit:  too much arguments \nexit: use: exit [val]",58);
		return 1;
	}
	free(prompt);
	clearTokenList(toklist);

	if(argc == 1){
		exit(ret_val);
	}
	if (argc == 2 && !(strcmp(argv[1],"0"))){
		exit(0);
	} else {
		exit(argv[1]);
	}
}
