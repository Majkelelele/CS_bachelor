/**
 * This file is for implementation of mimpirun program.
 * */


#include "mimpi_common.h" 
#include "channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
// #include <sys/mman.h>
// #include <time.h>

int numberOfDescriptorsNeeded = 0;
int specialReadPipe = -1;


void setEnvVar(int a, char *s) {
    char buffer[sizeof(int)];
    int ret = snprintf(buffer, sizeof buffer, "%d", a);
    if (ret < 0 || ret >= (int)sizeof(buffer))
        fatal("snprintf failed");
    setenv(s, buffer, 1);
}

void createChannels(int *pipe_dsc) {
    for(int i = 0; i < numberOfDescriptorsNeeded; i += 2) {
        ASSERT_SYS_OK(channel(pipe_dsc + i));
        if(specialReadPipe == -1 && pipe_dsc[i] >= firstAllowedPipe) specialReadPipe = pipe_dsc[i];
        // printf("first pipe: %d, second pipe: %d \n", pipe_dsc[i], pipe_dsc[i+1]);
    }
    for(int i = 0; i < numberOfDescriptorsNeeded; i++) {
        if(pipe_dsc[i] < specialReadPipe || pipe_dsc[i] >= numberOfDescriptorsNeeded) {
            // printf("closing unneded pipe %d \n", pipe_dsc[i]);
            ASSERT_SYS_OK(close(pipe_dsc[i]));
        }   
    }
}

void closeChannels(int *pipe_dsc) {
    for(int i = 0; i < numberOfDescriptorsNeeded; i++) {
        if(pipe_dsc[i] >= specialReadPipe && pipe_dsc[i] < numberOfDescriptorsNeeded) {
            ASSERT_SYS_OK(close(pipe_dsc[i]));
        }
    } 
}

// defining main with arguments
int main(int argc, char* argv[])
{
    int size = atoi(argv[1]);
    numberOfDescriptorsNeeded = 2*(size*size + 11 + size);
    for(int i = 20; i < 1024; i++) close(i);
    pid_t pid;
    int pipe_dsc[numberOfDescriptorsNeeded];
    createChannels(pipe_dsc);
    int firstBarierPipe = specialReadPipe + 2 + 2*size*size;

    
    setEnvVar(size,"size");
    setEnvVar(numberOfDescriptorsNeeded, "numberOfDescriptorsNeeded");
    setEnvVar(specialReadPipe,"specialReadPipe");
    setEnvVar(specialReadPipe+1,"specialWritePipe");
    setEnvVar(specialReadPipe+2,"firstCommonPipe");
    setEnvVar(firstBarierPipe,"firstBarierPipe");
    
    int rank = 0;

    while(size > 0) {
        
        setEnvVar(rank,"rank");
        
        rank++;
        ASSERT_SYS_OK(pid = fork());
        if (!pid) {
            ASSERT_SYS_OK(execvp(argv[2], &argv[2]));
        }
        size--;
    }
    closeChannels(pipe_dsc);


    size = atoi(argv[1]);

    for(int i = 0; i < size; i++) {
        ASSERT_SYS_OK(wait(NULL));
    }

    
    return 0;

}