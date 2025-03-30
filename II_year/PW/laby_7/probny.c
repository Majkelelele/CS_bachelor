#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdnoreturn.h>
#include "err.h"
#define BUFFER_SIZE 3


int main(int argc, char* argv[])
{
    int n_children = 3;
    if (argc > 2)
        fatal("Expected zero or one arguments, got: %d.", argc - 1);
    if (argc == 2)
        n_children = atoi(argv[1]);
    if(n_children > 0) {
        pid_t pid = fork();
        if(pid == 0) {
            printf("My pid is %d, my parent's pid is %d\n", getpid(), getppid());
        }
        if(!pid) {                
                char buffer[BUFFER_SIZE];
                int ret = snprintf(buffer, sizeof buffer, "%d", n_children-1);
                if (ret < 0 || ret >= (int)sizeof(buffer))
                    fatal("snprintf failed");
            char *args[] = {"./probny",buffer,NULL};
            execvp(args[0],args);
        }
        wait(NULL);
    }
    printf("My pid is %d, exiting.\n", getpid());

    return 0;
}