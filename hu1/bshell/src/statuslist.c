#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "statuslist.h"
#include <sys/types.h>
#include <sys/wait.h>

subprocess_t *subprocess_list = NULL;

void add_subprocess(pid_t pid, pid_t pgid, const char *command) {
    subprocess_t *new_subprocess = (subprocess_t *) malloc(sizeof(subprocess_t));
    new_subprocess->pid = pid;
    new_subprocess->pgid = pgid;
    strncpy(new_subprocess->prog, command, sizeof(new_subprocess->prog) - 1);
    new_subprocess->prog[sizeof(new_subprocess->prog) - 1] = '\0';
    new_subprocess->status = -1;
    new_subprocess->signal = -1;
    new_subprocess->sub_prev = NULL;
    new_subprocess->sub_next = NULL;

    if (subprocess_list == NULL) {
        subprocess_list = new_subprocess;
    } else {
        subprocess_t *current = subprocess_list;
        while (current->sub_next != NULL) {
            current = current->sub_next;
        }
        current->sub_next = new_subprocess;
        new_subprocess->sub_prev = current;
    }
}

void clean_processlist(void){
    subprocess_t *current = subprocess_list;

    while (current != NULL) {
        if (WIFSIGNALED(current->status) || WIFEXITED(current->status)){
            remove_subprocess(current->pid);
        }
        current = current->sub_next;
    }
}

void remove_subprocess(pid_t pid) {
    subprocess_t *current = subprocess_list;
    while (current != NULL) {
        if (current->pid == pid) {
            if (current->sub_prev == NULL) {
                subprocess_list = current->sub_next;
                if (current->sub_next != NULL) {
                    current->sub_next->sub_prev = NULL;
                }
            } else {
                current->sub_prev->sub_next = current->sub_next;
                if (current->sub_next != NULL) {
                    current->sub_next->sub_prev = current->sub_prev;
                }
            }
            free(current);
            break;
        }

        current = current->sub_next;
    }
}

void update_subprocess(pid_t pid, int status) {
    subprocess_t *current = subprocess_list;
    while (current != NULL) {
        //  pid_t result = waitpid(current->pid, &status, WNOHANG);
        if (current->pid == pid) {

            current->status = status;
        }

        current = current->sub_next;
    }
}

void set_signal(pid_t pid, int sig){
    subprocess_t *current = subprocess_list;

    while (current != NULL) {
        //  pid_t result = waitpid(current->pid, &status, WNOHANG);
        if (current->pid == pid) {

            current->signal = sig;
        }

        current = current->sub_next;
    }
}


void print_subprocess() {
    printf("%-10s %-10s %-10s %-10s\n", "PID", "PGID", "STATUS", "PROG");

    subprocess_t *current = subprocess_list;


    while (current != NULL) {

        char status_str[20];
        if (WIFSIGNALED(current->status)) {
            snprintf(status_str, sizeof(status_str), "signal(%d)", WTERMSIG(current->status));
        }else if (WIFEXITED(current->status)) {
            snprintf(status_str, sizeof(status_str), "exit(%d)", WEXITSTATUS(current->status));
        } else {
            snprintf(status_str, sizeof(status_str), "running");
        }


        printf("%-10d %-10d %-10s %-10s\n", current->pid, current->pgid, status_str, current->prog);

        current = current->sub_next;
    }

}

void sigchld_handler(int signum){
    int status;
    pid_t pid;
    while((pid = waitpid(-1, &status, WNOHANG))>0){
        update_subprocess(pid, status);
    }
}

void sig_act(void){
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if(sigaction(SIGCHLD, &sa, NULL) == -1){
        perror("sigaction failed");
        exit(1);
    }
}