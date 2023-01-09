#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "token.h"
#include "slasherr.h"


extern int ret_val;
extern char *prompt;
extern struct tokenList *toklist;


char* itoa(int val, int base){
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];
}
int is_a_number(char * string, int s_len){
	for(int i = 0 ; i < s_len; i++){
		if (isdigit(string[i]) == 0 ) return 0;
	}
	return 1;
}

int exec_exit(int *fdin, int *fdout, int *fderr, int argc, char **argv){
	if (argc > 2 || argc < 1 || (argc > 1 && is_a_number(argv[1], strlen(argv[1])) == 0 )) {

                free(argv);
		print_err("exit", EXIT_FAILED_MSG);
		return 1;
	}
	if(argc == 1) {
                free(toklist);
                free(argv);
                exit(ret_val);
        }
	if(argc == 2 && (!strcmp(argv[1], "0"))){
                free(argv);
                exit(0);
	}else{
		char * ret = argv[1];
		free(argv);
		exit(atoi(ret));
	}
			
}