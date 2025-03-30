/**
 * This file is for implementation of MIMPI library.
 * */

#include "channel.h"
#include "mimpi.h"
#include "mimpi_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

readData *listOfDataBeg = NULL;
readData *listOfDataEnd = NULL;
pthread_mutex_t globalMutex;
int finish = 1;
pthread_t threads[16];
pthread_cond_t tryTofind;



void freeBufer() {
    while(listOfDataBeg != NULL) {
        readData *toBeFreed = listOfDataBeg;
        free(toBeFreed->data);
        free(toBeFreed);
        listOfDataBeg = listOfDataBeg->next;
    }
}

int min(int a, int b) {
    if(a < b) return a;
    else return b;
}
int max(int a, int b) {
    if(a > b) return a;
    else return b;
}
readData *createMessageHolder(int count, int tag, int source) {
    readData *bufer = malloc(sizeof(readData));
    void *data = malloc(count);
    bufer->data = data;
    bufer->source = source;
    bufer->tag= tag;
    bufer->count = count;
    bufer->next = NULL;
    return bufer;
}

MIMPI_Retcode readFromPipe(int pipeNum, void *data, int size) {
    ssize_t read = chrecv(pipeNum, data, size);
    if(read == 0) {
        closeAllChannels();
        return MIMPI_ERROR_REMOTE_FINISHED;
    }
    ASSERT_SYS_OK(read);
    if (read != size) {
        fatal("read less than expected: %zd out of %zu bytes.\n", read, size);
    }
    return MIMPI_SUCCESS;
}

readData *findDataAndDelete(int count, int tag, int source) {
    readData *prior = listOfDataBeg;
    readData *current = listOfDataBeg->next;
    bool found = false;
    while(current != NULL && !found) {
        if(current->count == count && current->source == source && (current->tag == tag || tag == MIMPI_ANY_TAG)) found = true;
        else {
            prior = current;
            current = current->next;
        } 
    }
    if(current == NULL) return current;
    else {
        if(current == listOfDataEnd) listOfDataEnd = prior;
        prior->next = current->next;
    } 
    return current;
}

void *readMessageAndPutIntoBufer(void *data) {
    
        int pipeNum = *((int*)data);
        data = (void*)MIMPI_SUCCESS;
        while(true) {

            int count = 0;
            int tag = 0;
            int source = 0;
            if(readFromPipe(pipeNum,&count,sizeof(count)) != MIMPI_SUCCESS) data = (void*) MIMPI_ERROR_REMOTE_FINISHED;
            if(count == -1) {
                break; 
            } 
            if(readFromPipe(pipeNum,&tag,sizeof(tag)) != MIMPI_SUCCESS) data = (void*) MIMPI_ERROR_REMOTE_FINISHED;
            if(readFromPipe(pipeNum,&source,sizeof(source)) != MIMPI_SUCCESS) data = (void*)MIMPI_ERROR_REMOTE_FINISHED;
            readData *newBufer = createMessageHolder(count,tag,source);

            int sizeToBeRead = 0;
            void *data = newBufer->data;

            while(count > 0) {
                sizeToBeRead = min(pipeSizeBufer,count);
                if(readFromPipe(pipeNum,data,sizeToBeRead) != MIMPI_SUCCESS) data = (void*)MIMPI_ERROR_REMOTE_FINISHED;
                data = (void*)(data + sizeToBeRead);
                count -= sizeToBeRead;
            }
            newBufer->retcode = *((int*)data);

            ASSERT_ZERO(pthread_mutex_lock(&globalMutex));
            listOfDataEnd->next = newBufer;
            listOfDataEnd = newBufer;
            ASSERT_ZERO(pthread_cond_signal(&tryTofind));
            ASSERT_ZERO(pthread_mutex_unlock(&globalMutex));
        }
        
        return MIMPI_SUCCESS;
    
    
}

void initializeBufer() {
    listOfDataBeg = createMessageHolder(1,-1,-1);
    listOfDataBeg->source = -1;
    listOfDataEnd = listOfDataBeg;
}

MIMPI_Retcode sendToPipe(int pipeNum, void const *data, int size) {
    ssize_t wrote = chsend(pipeNum, data, size);
    if(wrote == -1) {
        return MIMPI_ERROR_REMOTE_FINISHED;
    } 
    ASSERT_SYS_OK(wrote);
    if (wrote != size) {
        fatal("Wrote less than expected: %zd out of %zu bytes.\n", wrote, size);
    }
    return MIMPI_SUCCESS;
}

void MIMPI_Init(bool enable_deadlock_detection) {
    channels_init();
    initializeBufer();
    ASSERT_ZERO(pthread_mutex_init(&globalMutex, NULL));
    ASSERT_ZERO(pthread_cond_init(&tryTofind, NULL));

    pthread_attr_t attr;
    ASSERT_ZERO(pthread_attr_init(&attr));
    ASSERT_ZERO(pthread_attr_setdetachstate(&attr,  PTHREAD_CREATE_JOINABLE));
    int worldRank = MIMPI_World_rank();
    int worldSize = MIMPI_World_size();
    int BarierPipe = atoi(getenv("firstBarierPipe"));


    int FirstCommonPipe = atoi(getenv("firstCommonPipe"));
    int numberOfDescriptorsNeeded = atoi(getenv("numberOfDescriptorsNeeded"));
    int minPipeNum = 2*worldRank*worldSize + FirstCommonPipe;
    int maxPipeNum = 2*(worldRank + 1)*worldSize + FirstCommonPipe - 1;


    for (int i = 0; i < worldSize; i++) {
        int* worker_arg = malloc(sizeof(int));
        *worker_arg = minPipeNum + 2*i;
        ASSERT_ZERO(pthread_create(&threads[i], &attr, readMessageAndPutIntoBufer, worker_arg));
    }

    for(int i = FirstCommonPipe; i <= numberOfDescriptorsNeeded; i += 2) {
        if((i < minPipeNum || i > maxPipeNum) && ((i - FirstCommonPipe)/2)%worldSize != worldRank) {
            if(i < BarierPipe) {
                close(i);
                close(i+1);
            }
            
            
          }     
        // if(i >= minPipeNum && i <= maxPipeNum) {
        //     // if(minPipeNum + 2*worldRank != i){
        //     //     close(i+1);
        //     //     printf("process %d closing file %d\n",worldRank, i+1);
        //     // } 
        // } 
    }
}

void MIMPI_Finalize() {
    int FirstCommonPipe = atoi(getenv("firstCommonPipe"));
    int minPipeNum = 2*MIMPI_World_rank()*MIMPI_World_size() + FirstCommonPipe;
    finish = 0;
    int stopThreads = -1;

    for (int i = 0; i < MIMPI_World_size(); i++) {
        sendToPipe(minPipeNum + 2*i + 1,&stopThreads,sizeof(int));
    }
    for (int i = 0; i < MIMPI_World_size(); i++) {
        int* result;
        ASSERT_ZERO(pthread_join(threads[i], (void**)&result));
        free(result);
    }
    
    ASSERT_ZERO(pthread_mutex_destroy(&globalMutex));
    ASSERT_ZERO(pthread_cond_destroy(&tryTofind));
    freeBufer();
    closeAllChannels();
    channels_finalize();
}

int MIMPI_World_size() {
    int sizeObtained = atoi(getenv("size"));
    return sizeObtained;
}

int MIMPI_World_rank() {
    int rank = atoi(getenv("rank"));
    return rank;
}







MIMPI_Retcode MIMPI_Send(
    void const *data,
    int count,
    int destination,
    int tag
) {
    
    int FirstCommonPipe = atoi(getenv("firstCommonPipe"));
    int source = MIMPI_World_rank();
    if(destination >= atoi(getenv("size")) || destination < 0) {
        return MIMPI_ERROR_NO_SUCH_RANK;
    }
    if(MIMPI_World_rank() == destination) return MIMPI_ERROR_ATTEMPTED_SELF_OP; 
    int pipeNum = FirstCommonPipe + 2*(destination*MIMPI_World_size() + MIMPI_World_rank()) + 1;


    if(sendToPipe(pipeNum,&count,sizeof(count)) != MIMPI_SUCCESS) return MIMPI_ERROR_REMOTE_FINISHED;

    if(sendToPipe(pipeNum,&tag,sizeof(tag)) != MIMPI_SUCCESS) return MIMPI_ERROR_REMOTE_FINISHED;

    if(sendToPipe(pipeNum,&source,sizeof(source)) != MIMPI_SUCCESS) return MIMPI_ERROR_REMOTE_FINISHED;

    int sizeToBeWritten = 0;
    while(count > 0) {
        sizeToBeWritten = min(pipeSizeBufer,count);
        if(sendToPipe(pipeNum,data,sizeToBeWritten) != MIMPI_SUCCESS) return MIMPI_ERROR_REMOTE_FINISHED;
        data = (void*)(data + sizeToBeWritten);
        count -= sizeToBeWritten;
    }    
    return MIMPI_SUCCESS;
    
}

MIMPI_Retcode MIMPI_Recv(
    void *data,
    int count,
    int source,
    int tag
) {
    

    if(source >= atoi(getenv("size")) || source < 0) {
        
        return MIMPI_ERROR_NO_SUCH_RANK;
    } 
    
    if(MIMPI_World_rank() == source) return MIMPI_ERROR_ATTEMPTED_SELF_OP;
    
    readData *fromBufer = NULL;
    ASSERT_ZERO(pthread_mutex_lock(&globalMutex));
    fromBufer = findDataAndDelete(count,tag,source);
    while (fromBufer == NULL) {
        ASSERT_ZERO(pthread_cond_wait(&tryTofind, &globalMutex));
        fromBufer = findDataAndDelete(count,tag,source);
    }
        
    ASSERT_ZERO(pthread_mutex_unlock(&globalMutex));

    void *tempData = fromBufer->data;

    memcpy(data, tempData, count);

    return MIMPI_SUCCESS;
}

MIMPI_Retcode MIMPI_Barrier() {
    

    int BarierPipe = atoi(getenv("firstBarierPipe"));
    int worldSize = MIMPI_World_size();
    int worldRank = MIMPI_World_rank();
    MIMPI_Retcode receive = MIMPI_SUCCESS;

    MIMPI_Retcode ret = MIMPI_SUCCESS;
    if(worldRank * 2 + 1< worldSize) {
        ret = readFromPipe(BarierPipe + 2*worldRank,&receive,sizeof(receive));
        if(receive != MIMPI_SUCCESS) ret = MIMPI_ERROR_REMOTE_FINISHED;
    } 
    if(worldRank * 2 + 2 < worldSize){
       ret = readFromPipe(BarierPipe + 2*worldRank,&receive,sizeof(receive));
       if(receive != MIMPI_SUCCESS) ret = MIMPI_ERROR_REMOTE_FINISHED;
    } 
    if(worldRank != 0) {
        ret = sendToPipe(BarierPipe + ((worldRank-1)/2)*2 + 1, &ret,sizeof(ret));
    } 
    if(worldRank != 0) {

        ret  = readFromPipe(BarierPipe + 2*worldRank,&receive, sizeof(receive));
        if(receive != MIMPI_SUCCESS) ret = MIMPI_ERROR_REMOTE_FINISHED;
    } 
    if(worldRank * 2 + 1 < worldSize) {
        ret  = sendToPipe(BarierPipe + (worldRank * 2 + 1)*2 + 1,&ret,sizeof(ret));
    } 
    if(worldRank * 2  + 2 < worldSize) {
        ret  = sendToPipe(BarierPipe + (2*worldRank + 2)*2 + 1,&ret,sizeof(ret));
    } 
    

    return ret;
}

MIMPI_Retcode MIMPI_Bcast(
    void *data,
    int count,
    int root
) {
    int BarierPipe = atoi(getenv("firstBarierPipe"));
    int worldSize = MIMPI_World_size();
    int worldRank = MIMPI_World_rank();

    if(worldRank == root) {
        worldRank = 0;
    }
    else if(worldRank == 0) {
        worldRank = root;
    }
    if(worldRank != 0) {
        readFromPipe(BarierPipe + 2*worldRank,data,count);
    }
    if(worldRank * 2 + 1 < worldSize) {
        sendToPipe(BarierPipe + (worldRank * 2 + 1)*2 + 1,data,count);
    } 
    if(worldRank * 2  + 2 < worldSize) {
        sendToPipe(BarierPipe + (2*worldRank + 2)*2 + 1,data,count);
    } 
    



    return MIMPI_SUCCESS;
}

MIMPI_Retcode MIMPI_Reduce(
    void const *send_data,
    void *recv_data,
    int count,
    MIMPI_Op op,
    int root
) {
    int BarierPipe = atoi(getenv("firstBarierPipe"));
    int worldSize = MIMPI_World_size();
    int worldRank = MIMPI_World_rank();
    u_int8_t *tab1;
    u_int8_t *tab2;
    u_int8_t *toBesend;
    toBesend = malloc(count);
    memcpy(toBesend, send_data, count);
    
    if(worldRank == root) {
        worldRank = 0;
    }
    else if(worldRank == 0) {
        worldRank = root;
    }
    if(worldRank * 2 + 1< worldSize) {
        tab1 = malloc(count);
        readFromPipe(BarierPipe + 2*worldRank,tab1,count);
        for(int i = 0; i < count; i++) {
            if(op == MIMPI_MAX) toBesend[i] = max(tab1[i],toBesend[i]);
            if(op == MIMPI_MIN) toBesend[i] = min(tab1[i],toBesend[i]);
            if(op == MIMPI_SUM) toBesend[i] = toBesend[i] + tab1[i];
            if(op == MIMPI_PROD) toBesend[i] = toBesend[i]*tab1[i];
        }
    } 
    if(worldRank * 2 + 2 < worldSize){
        tab2 = malloc(count);
        readFromPipe(BarierPipe + 2*worldRank,tab2,count);
        for(int i = 0; i < count; i++) {
            if(op == MIMPI_MAX) toBesend[i] = max(toBesend[i],tab2[i]);
            if(op == MIMPI_MIN) toBesend[i] = min(toBesend[i],tab2[i]);
            if(op == MIMPI_SUM) toBesend[i] = toBesend[i] + tab2[i];
            if(op == MIMPI_PROD) toBesend[i] = toBesend[i] * tab2[i];
        }
    } 
    
    if(worldRank != 0) {
        sendToPipe(BarierPipe + ((worldRank-1)/2)*2 + 1, toBesend,count);
    } 
    else {
        memcpy(recv_data, toBesend, count);
    }
    int check = 0;
    if(worldRank != 0) {
        readFromPipe(BarierPipe + 2*worldRank,&check,sizeof(check));
    } 
    if(worldRank * 2 + 1 < worldSize) {
        sendToPipe(BarierPipe + (worldRank * 2 + 1)*2 + 1,&check,sizeof(check));
    } 
    if(worldRank * 2  + 2 < worldSize) {
        sendToPipe(BarierPipe + (2*worldRank + 2)*2 + 1,&check,sizeof(check));
    } 
    free(toBesend);
    if(worldRank * 2 + 1< worldSize) free(tab1);
    if(worldRank * 2 + 2 < worldSize)free(tab2);

    return MIMPI_SUCCESS;
}