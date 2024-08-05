#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // For boolean values
#include <pthread.h>
#include <time.h>
#include <math.h>

#define MAX_VALUE 65536  // Range of random values

struct Node {
    int data;
    struct Node* next;
};

struct ThreadData{
    long thread_id;
    char* operations;
    long m;
};

void generate_unique_values(int* values, int n);
void createInitialLinkedList(int* values, int n);
struct Node* deleteList();
void createOperationList(char* operations,int m, float m_insert,float m_delete,float m_member);
void shuffleOperations(char* operations, int m);
void *threadOperation(void* thread_data);
int member(int value);
void insert(int value);
void delete(int value);
double calculate_mean(double* values, int size);
double calculate_standard_deviation(double* values, int size, double mean);
double calculate_required_samples(double stddev, double mean, double z, double error);

struct Node* head = NULL;  /*making the initial linked list*/
int thread_count;        /*variable to store the thread count from the command line*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  /*mutex variable*/
int n; /*number of nodes in the linked list*/
int m; /*number of operations to perform*/
float m_insert;
float m_delete;
float m_member;
bool used[MAX_VALUE] = {false}; /*array to store the used values*/
int total_operations = 0;

void printLinkedList() {
    int count = 0;
    struct Node* current = head;
    //printf("Linked List: ");
    while (current != NULL) {
        //printf("%d -> ", current->data);
        current = current->next;
        count++;
    }
    //printf("NULL\n");
    printf("Number of nodes: %d\n", count);
}

int main(int argc, char* arg[]){
    pthread_t *thread_list; /*array pointer of pthreads*/

    /*get the number of threads from the command line*/
    thread_count = strtol(arg[1], NULL, 10);
    n = strtol(arg[2], NULL, 10); /*items to linked list*/
    m = strtol(arg[3], NULL, 10); /*number of operations to perform*/
    m_member = strtof(arg[4], NULL); /*number of member operations*/
    m_insert = strtof(arg[5], NULL); /*number of insert operations*/
    m_delete = strtof(arg[6], NULL); /*number of delete operations*/

    int repetitions = 100; // initial number of repetitions
    double execution_times[repetitions];  /*list ti store the execution times*/
    double total_time, mean, stddev, required_samples;   /*variables to calculate the stats*/

    
    /*generate unique values*/
    int* unique_values = malloc(n * sizeof(int));
    generate_unique_values(unique_values, n);

    /*creating initial linked list*/
    createInitialLinkedList(unique_values,n);


    // printf("Initial Linked List:\n");
    // printLinkedList();

    /*dividing operations*/
    char operations[m];
    createOperationList(operations, m, m_insert, m_delete, m_member);

    /*shuffling the operations*/
    shuffleOperations(operations, m);
    
    /*allocate memory for the array of pthreads*/
    thread_list = malloc(thread_count * sizeof(pthread_t));
    /*allocate memory for ThreadDataList*/
    struct ThreadData* thread_data = malloc(thread_count * sizeof(struct ThreadData));

    /*initialize the mutex*/
    if(pthread_mutex_init(&mutex, NULL) != 0){
        fprintf(stderr, "Error initializing mutex\n");
        exit(EXIT_FAILURE);
    }

    printf("Running the programme with %d threads and %d times to calculate sample size...\n\n", thread_count, repetitions);

    for (int i = 0; i < repetitions; i++) {
        if(i>0){
            /*delete linked list created for sample count*/
            head =  deleteList(head);

            /*creating initial linked list*/
            createInitialLinkedList(unique_values,n);

            clock_t start = clock();
        
            /*create the threads*/
            for(int i = 0; i < thread_count; i++){
                thread_data[i].thread_id = i;
                thread_data[i].operations = operations;
                thread_data[i].m = m;
                pthread_create(&thread_list[i], NULL, threadOperation, (void*) &thread_data[i]);
            }

            /*join the threads*/
            for(int i = 0; i < thread_count; i++){
                pthread_join(thread_list[i], NULL);
            }


            clock_t end = clock();
            execution_times[i] = ((double)(end - start)) / CLOCKS_PER_SEC;
        }else{
            clock_t start = clock();
        
            /*create the threads*/
            for(int i = 0; i < thread_count; i++){
                thread_data[i].thread_id = i;
                thread_data[i].operations = operations;
                thread_data[i].m = m;
                pthread_create(&thread_list[i], NULL, threadOperation, (void*) &thread_data[i]);
            }

            /*join the threads*/
            for(int i = 0; i < thread_count; i++){
                pthread_join(thread_list[i], NULL);
            }


            clock_t end = clock();
            execution_times[i] = ((double)(end - start)) / CLOCKS_PER_SEC;
        }
        
    }

    // printf("After 100 Linked List:\n");
    // printLinkedList();

    /*delete linked list created for sample count*/
    head =  deleteList(head);

    /*creating initial linked list*/
    createInitialLinkedList(unique_values,n);

    // printf("second Final Linked List:\n");
    // printLinkedList();

    mean = calculate_mean(execution_times, repetitions);   /*calculate the mean of the ran 100 execution*/
    stddev = calculate_standard_deviation(execution_times, repetitions, mean);   /*calculate standard deviation of the samples*/
    
    double z = 1.960; // 95% confidence level
    double error = 0.05 * mean; // desired accuracy in seconds
    
    required_samples = calculate_required_samples(stddev, mean, z, error);   /*calculate the required samples to get the desired confidence accuracy*/
    
    printf("Required number of samples: %f\n", required_samples);
    printf("runnning the programme with the required number of samples...\n\n");

    double realExecutionTimes[(int)required_samples];

    for(int i = 0; i < (int)required_samples; i++){
        

        if(i>0){
            /*delete linked list created for sample count*/
            head =  deleteList(head);

            /*creating initial linked list*/
            createInitialLinkedList(unique_values,n);

            clock_t start = clock();
        
            /*create the threads*/
            for(int i = 0; i < thread_count; i++){
                thread_data[i].thread_id = i;
                thread_data[i].operations = operations;
                thread_data[i].m = m;
                pthread_create(&thread_list[i], NULL, threadOperation, (void*) &thread_data[i]);
            }

            /*join the threads*/
            for(int i = 0; i < thread_count; i++){
                pthread_join(thread_list[i], NULL);
            }

            clock_t end = clock();
            realExecutionTimes[i] = ((double)(end - start)) / CLOCKS_PER_SEC;
        }else{
            clock_t start = clock();
        
            /*create the threads*/
            for(int i = 0; i < thread_count; i++){
                thread_data[i].thread_id = i;
                thread_data[i].operations = operations;
                thread_data[i].m = m;
                pthread_create(&thread_list[i], NULL, threadOperation, (void*) &thread_data[i]);
            }

            /*join the threads*/
            for(int i = 0; i < thread_count; i++){
                pthread_join(thread_list[i], NULL);
            }

            clock_t end = clock();
            realExecutionTimes[i] = ((double)(end - start)) / CLOCKS_PER_SEC;
        }
        
    }

    mean = calculate_mean(realExecutionTimes, (int)required_samples);   /*calculate the mean of the ran 100 execution*/
    stddev = calculate_standard_deviation(realExecutionTimes, (int)required_samples, mean);   /*calculate standard deviation of the samples*/
    printf("Mean execution time: %f seconds, std: %f for samples: %d , total_operations: %d", mean,stddev, (int)required_samples, total_operations);
    

    /*free the memory*/
    free(thread_list);
    free(thread_data);
    free(unique_values);
    

    /*destroy the mutex*/
    pthread_mutex_destroy(&mutex);

    return 0;

}

void generate_unique_values(int* values, int n) {
    
    int count = 0;
    
    while (count < n) {
        int value = rand() % MAX_VALUE;
        if (!used[value]) {
            used[value] = true;
            values[count++] = value;
        }
    }
}

void createInitialLinkedList(int* values, int n){
    for(int i=0;i<n;i++){
        struct Node* new_node = malloc(sizeof(struct Node));
        new_node->data = values[i];
        new_node->next = head;
        head = new_node;
    }
}

struct Node* deleteList(){
    struct Node* current = head;
    struct Node* next;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    struct Node* head = NULL;
    return head;
}

void createOperationList(char* operations,int m, float m_insert,float m_delete,float m_member){
    int insert = m * m_insert;
    int delete = m * m_delete;
    int member = m * m_member;
    for(int i = 0; i < insert; i++){
        operations[i] = 'I';
    }
    for(int i = insert; i < insert + delete; i++){
        operations[i] = 'D';
    }
    for(int i = insert + delete; i < insert + delete + member; i++){
        operations[i] = 'M';
    }
    
}

void shuffleOperations(char* operations, int m){
    for(int i = 0; i < m; i++){
        int j = rand() % m;
        char temp = operations[i];
        operations[i] = operations[j];
        operations[j] = temp;
    }
}

void *threadOperation(void* thread_data){
    struct ThreadData* data = (struct ThreadData*) thread_data;   
    long my_rank = data->thread_id;
    char* operations = data->operations;
    long my_m = data->m;

    int start = (m / thread_count) * my_rank;
    int end = (m / thread_count) * (my_rank + 1);

    for(int i = start; i<end;i++){
        int random_value = rand() % MAX_VALUE;
        if(operations[i] == 'M'){
            pthread_mutex_lock(&mutex);
            member(random_value);
            pthread_mutex_unlock(&mutex);
        }else if(operations[i] == 'I'){
            pthread_mutex_lock(&mutex);
            insert(random_value);
            pthread_mutex_unlock(&mutex);
        }else{
            pthread_mutex_lock(&mutex);
            delete(random_value);
            pthread_mutex_unlock(&mutex);
        }
    }
}

int member(int value) {
    total_operations++;
    //printf("Member: %d\n", value);
    struct Node* current = head;
    
    while (current != NULL) {
        if (current->data == value) {
            return 1;
        }
        current = current->next;
    }
 
    return 0;
}

void insert(int value){
    total_operations++;
    //printf("Insert: %d\n", value);
    /*here for the simplicity of the execution we insert the nodes to the head of the linked list*/
 
    struct Node* new_node = malloc(sizeof(struct Node));
    if (!new_node) {
        perror("Failed to allocate memory for new node");
        return;
    }
    new_node->data = value;
    new_node->next = NULL;

    struct Node* current_node = head;
    struct Node* previous_node = NULL;

    while (current_node != NULL && current_node->data < value) {
        previous_node = current_node;
        current_node = current_node->next;
    }
    if (previous_node == NULL) {
        new_node->next = head;
        head = new_node;
    } else {
        new_node->next = current_node;
        previous_node->next = new_node;
    }

    
}

void delete(int value) {
    total_operations++;
    //printf("Delete: %d\n", value);
    struct Node* current = head;
    if (current == NULL) { // Empty list
        return;
    }
    if (current->data == value) { // Delete the head
        head = current->next;
        free(current);
        return;
    }
    struct Node* previous = current;
    current = current->next;

    while (current != NULL && current->data != value) { // Find the node to delete
        previous = current;
        current = current->next;
    }

    // If the node is not found
    if (current == NULL) { 
        return;
    }
    previous->next = current->next;  // Delete the node
    free(current);
}

double calculate_mean(double* values, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += values[i];
    }
    return sum / size;
}

double calculate_standard_deviation(double* values, int size, double mean) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += pow(values[i] - mean, 2);
    }
    return sqrt(sum / size);
}

double calculate_required_samples(double stddev, double mean, double z, double error) {
    return pow((z * stddev) / error, 2);
}
