#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "shell.h"
#include "helper.h"
#include "command.h"
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "statuslist.h"
#include "debug.h"
#include "execute.h"

/* do not modify this */
#ifndef NOLIBREADLINE

#include <readline/history.h>

#endif /* NOLIBREADLINE */

extern int shell_pid;
extern int fdtty;

/* do not modify this */
#ifndef NOLIBREADLINE

int chld_return_value = 0;


static int builtin_hist(char **command) {

    register HIST_ENTRY **the_list;
    register int i;
    printf("--- History --- \n");

    the_list = history_list();
    if (the_list)
        for (i = 0; the_list[i]; i++)
            printf("%d: %s\n", i + history_base, the_list[i]->line);
    else {
        printf("history could not be found!\n");
    }

    printf("--------------- \n");
    return 0;
}

#endif /*NOLIBREADLINE*/

void unquote(char *s) {
    if (s != NULL) {
        if (s[0] == '"' && s[strlen(s) - 1] == '"') {
            char *buffer = calloc(sizeof(char), strlen(s) + 1);
            strcpy(buffer, s);
            strncpy(s, buffer + 1, strlen(buffer) - 2);
            s[strlen(s) - 2] = '\0';
            free(buffer);
        }
    }
}

void unquote_command_tokens(char **tokens) {
    int i = 0;
    while (tokens[i] != NULL) {
        unquote(tokens[i]);
        i++;
    }
}

void unquote_redirect_filenames(List *redirections) {
    List *lst = redirections;
    while (lst != NULL) {
        Redirection *redirection = (Redirection *) lst->head;
        if (redirection->r_type == R_FILE) {
            unquote(redirection->u.r_file);
        }
        lst = lst->tail;
    }
}

void unquote_command(Command *cmd) {
    List *lst = NULL;
    switch (cmd->command_type) {
        case C_SIMPLE:
        case C_OR:
        case C_AND:
        case C_PIPE:
        case C_SEQUENCE:
            lst = cmd->command_sequence->command_list;
            while (lst != NULL) {
                SimpleCommand *cmd_s = (SimpleCommand *) lst->head;
                unquote_command_tokens(cmd_s->command_tokens);
                unquote_redirect_filenames(cmd_s->redirections);
                lst = lst->tail;
            }
            break;
        case C_EMPTY:
        default:
            break;
    }
}

void signal_callback_handler(int signum) {
    printf("\n");
    chld_return_value = signum;
}

int check_redirections(SimpleCommand *cmd_s) {
    if (cmd_s->redirections != NULL) {
        List *reds = cmd_s->redirections;
        int red_cnt = 0;
        do {
            red_cnt++;
            reds = reds->tail;
        } while (reds != NULL);

        red_cnt = 1;
        reds = cmd_s->redirections;
        int fd;
        do {
            Redirection *red = (Redirection *) reds->head;

            if (red->r_type == R_FD) {
                if (red->r_mode == M_READ || red->r_mode == M_WRITE || red->r_mode == M_APPEND) {
                    dup2(red->u.r_fd, STDOUT_FILENO);
                    close(red->u.r_fd);
                } else {
                    fprintf(stderr, "wrong way!\n");
                    exit(1);
                }
            } else if (red->r_type == R_FILE) {
                if (red->r_mode == M_READ) {             //read from file
                    fd = open(red->u.r_file, O_RDONLY | O_CLOEXEC);
                    if (fd == -1) {
                        perror(red->u.r_file);
                        return 1;
                    }
                    dup2(fd, 0);
                } else if (red->r_mode == M_WRITE) {      //write to file
                    fd = open(red->u.r_file, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0666);
                    if (fd == -1) {
                        perror(red->u.r_file);
                        return 1;
                    }
                    dup2(fd, 1);
                } else if (red->r_mode == M_APPEND) {     //append to file
                    fd = open(red->u.r_file, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC, 0666);
                    if (fd == -1) {
                        perror(red->u.r_file);
                        return 1;
                    }
                    dup2(fd, 1);
                } else {
                    exit(1);
                }
            } else {
                exit(1);
            }

            reds = reds->tail;
        } while (reds != NULL);
        return 0;
    }
    return 0;
}

static int execute_fork(SimpleCommand *cmd_s, int background) {
    int status;
    char **command = cmd_s->command_tokens;
    pid_t pid, wpid;
    //signal(SIGCHLD, signal_callback_handler);
    pid = fork();
    if (pid == 0) {
        //sig_act();
        /* child */
        signal(SIGINT, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        setpgid(pid, pid);
        /*
         * handle redirections here
         */


        if (check_redirections(cmd_s) != 0) {
            exit(EXIT_FAILURE);
        }

        execvp(command[0], command);
        fprintf(stderr, "-bshell: %s : command not found \n", command[0]);
        perror("");


        /*exec only return on error*/
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("shell");

    } else {
        /*parent*/
        add_subprocess(pid, getpgid(pid), command[0]);
        setpgid(pid, pid);
        if (background == 0) {
            /* wait only if no background process */
            tcsetpgrp(fdtty, pid);

            /**
             * the handling here is far more complicated than this!
             * vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
             */
            wpid = waitpid(pid, &status, 0);

            //^^^^^^^^^^^^^^^^^^^^^^^^^^^^

            tcsetpgrp(fdtty, shell_pid);
            update_subprocess(pid, status);            

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                return 0;
            }
            return 1;
        } else {

            fprintf(stderr, "PID: %d\tPGID: %d\n", pid, getpgid(pid));
        }
    }

    return WEXITSTATUS(status);
}

static int do_execute_simple(SimpleCommand *cmd_s, int background) {
    if (cmd_s == NULL) {
        return 0;
    } else if (strcmp(cmd_s->command_tokens[0], "cd") == 0) {
        if (cmd_s->command_token_counter == 1) {
            chdir(getenv("HOME"));
        } else {
            char *set_path = cmd_s->command_tokens[1];
            if (chdir(cmd_s->command_tokens[1]) != 0) {
                if (errno == 2) {
                    printf("%s: No such file or directory\n", cmd_s->command_tokens[1]);
                } else if (errno == 13) {
                    printf("%s: permission denied\n", cmd_s->command_tokens[1]);
                }
            }
        }

        return 0;
    } else if (strcmp(cmd_s->command_tokens[0], "secho") == 0) {
        printf("%s\n", cmd_s->command_tokens[1]);
        return 0;
    } else if (strcmp(cmd_s->command_tokens[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(cmd_s->command_tokens[0], "status") == 0) {
        print_subprocess();
        clean_processlist();

        return 0;



/* do not modify this */
#ifndef NOLIBREADLINE
    } else if (strcmp(cmd_s->command_tokens[0], "hist") == 0) {
        return builtin_hist(cmd_s->command_tokens);
#endif /* NOLIBREADLINE */
    } else {
        return execute_fork(cmd_s, background);
    }
    fprintf(stderr, "This should never happen!\n");
    exit(1);
}

/*
 * check if the command is to be executed in back- or foreground.
 *
 * For sequences, the '&' sign of the last command in the
 * sequence is checked.
 *
 * returns:
 *      0 in case of foreground execution
 *      1 in case of background execution
 *
 */
int check_background_execution(Command *cmd) {
    List *lst = NULL;
    int background = 0;
    switch (cmd->command_type) {
        case C_SIMPLE:
            lst = cmd->command_sequence->command_list;
            background = ((SimpleCommand *) lst->head)->background;
            break;
        case C_OR:
        case C_AND:
        case C_PIPE:
        case C_SEQUENCE:
            /*
             * last command in sequence defines whether background or
             * forground execution is specified.
             */
            lst = cmd->command_sequence->command_list;
            while (lst != NULL) {
                background = ((SimpleCommand *) lst->head)->background;
                lst = lst->tail;
            }
            break;
        case C_EMPTY:
        default:
            break;
    }
    return background;
}


int execute(Command *cmd) {
    unquote_command(cmd);

    int res = 0;
    List *lst = NULL;
    int execute_in_background = check_background_execution(cmd);
    /**
     * if(execute_in_background == 1){
     * printchildid}
     */
    SimpleCommand *cmd_s = (SimpleCommand *) cmd->command_sequence->command_list->head;

    switch (cmd->command_type) {
        case C_EMPTY:
            break;
        case C_SIMPLE:
            res = do_execute_simple(cmd_s, execute_in_background);
            fflush(stderr);
            break;

        case C_OR:
            lst = cmd->command_sequence->command_list;

            do {
                SimpleCommand *cmd_s = (SimpleCommand *) lst->head;
                res = do_execute_simple(cmd_s, execute_in_background);
                lst = lst->tail;
                if (lst == NULL) break;
            } while (res != 0);

            break;
        case C_AND:
            lst = cmd->command_sequence->command_list;

            do {
                SimpleCommand *cmd_s = (SimpleCommand *) lst->head;
                res = do_execute_simple(cmd_s, execute_in_background);
                lst = lst->tail;
                if (lst == NULL) break;
            } while (res == 0);

            break;
        case C_SEQUENCE:
            lst = cmd->command_sequence->command_list;
            while (lst != NULL) {
                SimpleCommand *cmd_s = (SimpleCommand *) lst->head;
                res = do_execute_simple(cmd_s, execute_in_background);
                lst = lst->tail;
            }
            break;
        case C_PIPE: {
            pid_t pid, wpid;
            lst = cmd->command_sequence->command_list;
            int num_pipelines = cmd->command_sequence->command_list_len - 1;
            int cmd_count = cmd->command_sequence->command_list_len;

            int fds[num_pipelines][2]; // File descriptors for pipes
            pid_t pids[cmd_count];    // Array to store child process IDs

            // Create the pipes
            for (int i = 0; i < num_pipelines; i++) {
                if (pipe2(fds[i], O_CLOEXEC) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }

            // Fork child processes
            for (int i = 0; i < cmd_count; i++) {
                SimpleCommand *command = (SimpleCommand *) lst->head;
                char **tokens = command->command_tokens;
                pid = fork();

                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Child process
                    signal(SIGINT, SIG_DFL);
                    signal(SIGTTOU, SIG_DFL);
                    if (i > 0) {
                        // Not the first process, set up input from previous process
                        //close(fds[i - 1][1]);                              // Close write end of previous pipe
                        dup2(fds[i - 1][0], STDIN_FILENO);                 // Set read end of previous pipe as input
                    }

                    if (i < num_pipelines) {
                        // Not the last process, set up output to the next process
                        //close(fds[i][0]);                                  // Close read end of current pipe
                        dup2(fds[i][1], STDOUT_FILENO);                    // Set write end of current pipe as output
                    }

                    if ((execvp(tokens[0], tokens)) == -1) {
                        perror("Execvp error\n");

                    }
                } else {
                    pids[i] = pid;
                    /**
                     * parent group id gesetzt.
                     *
                     */
                    setpgid(pid, pids[0]);
                    tcsetpgrp(fdtty, pids[0]);

                }
                lst = lst->tail;
                fflush(stderr);
            }
            // Close all pipe ends in the parent process
            for (int i = 0; i < num_pipelines; i++) {
                close(fds[i][0]);
                close(fds[i][1]);
            }

            // Wait for all child processes to finish
            for (int i = 0; i < cmd_count; i++) {
                waitpid(pids[i], NULL, 0);
            }

            tcsetpgrp(fdtty, shell_pid);
            break;
        }
        default:
            printf("[%s] unhandled command type [%i]\n", __func__, cmd->command_type);
            break;
    }
    chld_return_value = 0;
    return res;
}