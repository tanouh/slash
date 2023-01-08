#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "external.h"
#include "slasherr.h"
#include "redirection.h"

pid_t child_pid;


char **formate_args(int argc, char **argv) {
        char **arg_list = malloc((argc + 1) * sizeof(char *));
        if (arg_list == NULL) {
                print_err(NULL, MALLOC_ERR);
                return NULL;
        }
        for (int i = 0; i < argc; i++) {
                arg_list[i] = argv[i];
        }
        arg_list[argc] = NULL;
        return arg_list;
}


void handler(int sig) {

        if (getpid() == child_pid) {
                // we are in the child process, so kill the child process
                switch (sig) {
                        case SIGINT:
                                kill(child_pid, SIGINT);
                                break; //sinon pid stocker
                        case SIGTERM:
                                kill(child_pid, SIGTERM);
                                break;
                        case SIGKILL:
                                kill(child_pid, SIGKILL);
                                break;
                }
        }

}

int exec_external(int fdin, int fdout, int fderr, int argc, char **argv) {
        if (argc < 0) {
                free(argv);
                return 1;
        }
        char **args_list = formate_args(argc, argv);
        if (args_list == NULL) {
                free(args_list);
                free(argv);
                exit(errno);
        };

        struct sigaction sa;
        memset(&sa, 0, sizeof(struct sigaction));
        sa.sa_handler = handler;

        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGKILL, &sa, NULL);

        pid_t proc = fork();

        int stat = -1;


        switch (proc) {
                case -1:
                        free(args_list);
                        free(argv);
                        exit(errno);
                case 0:

                        redirect(fdin, STDIN_FILENO); /*REDIR*/
                        redirect(fdout, STDOUT_FILENO);
                        redirect(fderr, STDERR_FILENO);

                        child_pid = getpid();

                        int status = execvp(argv[0], args_list);
                        if (status == -1) {
                                print_err(argv[0], "command not found");
                                free(args_list);
                                free(argv);
                                exit(errno);
                        }
                default:
                        waitpid(child_pid, &stat, 0);
                        wait(NULL);
                        free(args_list);
                        free(argv);
                        return WEXITSTATUS(stat);
        }
}
