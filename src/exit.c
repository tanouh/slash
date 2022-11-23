#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "token.h"

#define MSG_SIZE 100
#define EXIT_SUCCEED_MSG "-slash terminate with a return value : "
#define EXIT_FAILED_MSG "-slash: exit: failed \nexit: use: exit [val]\n"
#define ESM_LEN strlen(EXIT_SUCCEED_MSG)
#define EFM_LEN strlen(EXIT_FAILED_MSG)

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

int exec_exit(int argc, char **argv){
        if (argc > 1 || argc < 0 || (argc > 0 && is_a_number(argv[0], strlen(argv[0])) == 0 )) {
		// write(STDERR_FILENO,"%d", argc);
		write(STDERR_FILENO,EXIT_FAILED_MSG,EFM_LEN);
		return 1;
	}
	if(argc == 0) {
                free(toklist);
                exit(ret_val);
        }
	if(argc == 1 && (!strcmp(argv[0], "0"))){
		exit(0);
	}else{
		exit(atoi(argv[0]));
	}
			
}
	// char * exit_message = malloc(sizeof(MSG_SIZE));
	// if(exit_message == NULL){
	// 	write(STDERR_FILENO,EXIT_FAILED_MSG,EFM_LEN);
	// 	return 0;

	// }
	// memmove(exit_message,EXIT_SUCCEED_MSG, ESM_LEN);
	
	// if (argc == 1){
	// 	ret_val = atoi(argv[0]);
	// }

	//Exit message formatage and print
	// char *p = itoa(ret_val, 10);
	// size_t msg_len = ESM_LEN + strlen(p);
	// memmove(exit_message+ESM_LEN,p,msg_len);

	// write(STDERR_FILENO, EXIT_SUCCEED_MSG,ESM_LEN);
	// write(STDERR_FILENO, "\n", 1);

	// free(exit_message);
	//free(prompt);
	//clearTokenList(toklist);
	// free(toklist);
	
