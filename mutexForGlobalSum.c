#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

/*global variable*/
int thread_count;
int sum = 0;
int n = 10000;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int mutexInitializer() {
    return pthread_mutex_init(&mutex, NULL);
}


/*thread function*/
void *Hello(void* rank);

/*global sum function*/
void *GlobalSum(void* rank);

int main(int argc, char* args[]){
    long thread;
    pthread_t* thread_handles;  /*array pointer of pthreads*/

    /*get number of threads form the command line*/
    thread_count = strtol(args[1], NULL, 10);

    /*allocate memory for the array of pthreads*/
    thread_handles = malloc(thread_count * sizeof(pthread_t));


     // Initialize the mutex
    if (mutexInitializer() != 0) {
        fprintf(stderr, "Error initializing mutex\n");
        exit(EXIT_FAILURE);
    }

    /*create the threads*/
    for(thread = 0; thread< thread_count; thread++){
        pthread_create(&thread_handles[thread], NULL, GlobalSum, (void*) thread);
    }

    printf("global sum = %d\n", sum);   /*this will print 0 because the threads are not done yet*/

    /*join the threads*/
    for(thread = 0; thread< thread_count; thread++){
        pthread_join(thread_handles[thread], NULL);
    }

    printf("global sum = %d\n", sum);   /*this will print the correct sum*/

    /*free the memory*/
    free(thread_handles);

    // Destroy the mutex
    pthread_mutex_destroy(&mutex);
    
    return 0;
}


void *Hello(void* rank){
    long myrank = (long) rank;
    printf("Hello from thread %ld of %d\n", myrank, thread_count);

    return NULL;
}

void *GlobalSum(void* rank){
    long myRank = (long) rank;

    int i;
    int muSum = 0;
    int myN = (int) floor(n/thread_count);
    int myFirstI = myN * myRank;
    int myLastI = myFirstI + myN;
    for(i=myFirstI; i<myLastI; i++){
        muSum += i;
    }

    pthread_mutex_lock(&mutex);
    sum += muSum;
    pthread_mutex_unlock(&mutex);
    return NULL;

}

