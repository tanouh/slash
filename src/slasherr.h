#ifndef PROJET_SY5_SLASHERR_H
#define PROJET_SY5_SLASHERR_H

#define EXIT_FAILED_MSG "failed \nexit: use: exit [val]\n"
#define MALLOC_ERR "malloc: Memory not available"
#define PWD_ERR "Couldn't "


void print_err(char * cmd , char * err_msg);
void print_err_fd(int fderr, char *cmd, char *err_msg);
#endif