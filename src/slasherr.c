#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slasherr.h"

void print_err(char *cmd, char *err_msg) {
        if (cmd == NULL) {
                dprintf(STDERR_FILENO, "-slash : %s\n", err_msg);

        } else {
                dprintf(STDERR_FILENO, "-slash : %s: %s\n", cmd, err_msg);
        }
}

void print_err_fd(int fderr, char *cmd, char *err_msg) {
        if (cmd == NULL) {
                dprintf(fderr, "-slash : %s\n", err_msg);

        } else {
                dprintf(fderr, "-slash : %s: %s\n", cmd, err_msg);
        }
}