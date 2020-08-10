// CS3104 P2 starter code
// kt54, Oct 2018
//
// This program will start two processes and then toggle them using
// STOP and CONT signals 

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>


struct process {
    pid_t pid;
    int priority;
    char* path;
    char* parameters;
    struct process *next;
};



struct process * add(int prio,char* pat, char* par,  struct process *head) {
    struct process *new_node;
    new_node = ( struct process *) malloc(sizeof( struct process));
    new_node->priority = prio;
    new_node->path = pat;
    new_node->parameters = par;
    if(head-> path != NULL){
    new_node->next= head;
    }
    head = new_node;
    return head;
    free(new_node);
}

int count(struct process *head) {
    int cnt = 0;
    struct process *current_node = head;
    while ( current_node != NULL) {
        cnt++;
        current_node = current_node->next;
    }
    return(cnt);
}

void pront(struct process *head) {
    struct process *current_node = head;
   	while ( current_node != NULL) {
        printf("%d , %s , %s ", current_node->priority, current_node->path, current_node->parameters);
        printf("\n");
        current_node = current_node->next;
    }
}

struct process* fileReader(char *filename , struct process* head){
        FILE *file = fopen(filename , "r");
        char line[124];
        // can be put outside and passed as argument.
        char* priority,*path,*parameters;

        while (fgets(line, sizeof(line), file)) {
             priority=strtok(line," ");
             char *path_token = strtok(NULL, " ");
             path = malloc(strlen(path_token) + 1); /* remember to allocate space for \0 terminator! */
             strcpy(path, path_token);
             char *par_token = strtok(NULL, "\n");
             if(par_token == NULL){
                par_token = "";
             }
             parameters = malloc(strlen(par_token) + 1); /* remember to allocate space for \0 terminator! */
             strcpy(parameters, par_token);
             char* junk;
             int p = strtol(priority,&junk,10);
             head = add(p, path, parameters,head);

        }
        fclose(file);
        return head;
        free(parameters);
        free(path);
}


int main(int argc, char **argv){

    if(argc  < 2){
        printf("\nError not enough arguments\n");
        exit(0);
    } else if(access( argv[1], F_OK ) == -1){
        printf("\nFILE NOT FOUND\n");
        exit(0);
    } else{
        printf("\n");
        struct process *head = ( struct process *)malloc(sizeof(struct process));
        head-> priority = 0;
        head-> path = NULL;
        head-> parameters = NULL;
        head-> next = NULL;
        head = fileReader(argv[1], head);
        pront(head);

        printf("\n");

        struct process *current_node = head;
       	while ( current_node != NULL) {
                pid_t propid = 0;
                propid = fork();

                if(propid < 0)
                   printf("Something went wrong!\n");
                else if(propid > 0)    // We are parent. Immediately stop the new process
                   kill(propid,SIGSTOP);
                else{
                    execl(current_node->path, current_node->path, current_node->parameters, NULL);

                }  // We are a child process -- overwrite our process space with the new program
                current_node->pid = propid;
                current_node = current_node->next;
        }

//    struct process *other_node = head;
//        while ( other_node != NULL) {
//                int status;
//                pid_t result = waitpid( other_node->pid, &status, WNOHANG );
//                kill(other_node->pid, SIGCONT);        // Start P1
//                while (waitpid(other_node->pid, &status, 0) != -1) {
//                }
//                printf("\n");
//                kill(other_node->pid, SIGSTOP);
//                kill(other_node->pid, SIGTERM);
//                other_node = other_node->next;
//        }

        struct process *other_node = head;
        int i = 0;
        int counter = count(head);

            while ( other_node != NULL) {
                    int status;
                    pid_t result = waitpid( other_node->pid, &status, WNOHANG );
                    if(kill(other_node->pid,0) == 0){
                    kill(other_node->pid, SIGCONT);        // Start P1
                    usleep(400000);
                    printf(" ");
                    kill(other_node->pid, SIGSTOP);

                    } else{
                    i++;
                    kill(other_node->pid, SIGTERM);
                    }
                    if (other_node->next == NULL && i!= counter)   {
                       other_node = head;
                    }else{
                    other_node = other_node->next;
                    }

            }


        printf("\ncount   : %d",counter);
        printf("\nFinishing...\n");
        printf("\n");
        free(head);

    }
}


