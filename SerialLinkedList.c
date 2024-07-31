#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>  // For boolean values
#include <time.h>
#include <math.h>

#define MAX_VALUE 65536  // Range of random values

struct Node{
    int data;
    struct Node* next;
};  

void generate_unique_values(int* values, int n);
void createInitialLinkedList(int* values, int n);
void createOperationList(char* operations,int m, float m_insert,float m_delete,float m_member);
void shuffleOperations(char* operations, int m);
int member(int value);
void insert(int value);
void delete(int value);
double calculate_mean(double* values, int size);
double calculate_standard_deviation(double* values, int size, double mean);
double calculate_required_samples(double stddev, double mean, double z, double error);

struct Node* head = NULL;  /*making the initial linked list*/
int thread_count;        /*variable to store the thread count from the command line*/
int n; /*number of nodes in the linked list*/
int m; /*number of operations to perform*/
float m_insert;
float m_delete;
float m_member;

void printLinkedList() {
    struct Node* current = head;
    printf("Linked List: ");
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

int main(int argc, char* arg[]){
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

    printf("Running the programme with %d threads and %d times to calculate sample size...\n\n", thread_count, repetitions);

    for (int i = 0; i < repetitions; i++) {
        clock_t start = clock();
        
        for (int j = 0; j < m; j++) {
            int value = rand() % MAX_VALUE;
            switch (operations[j]) {
                case 'I':
                    insert(value);
                    break;
                case 'D':
                    delete(value);
                    break;
                case 'M':
                    member(value);
                    break;
            }
        }

        clock_t end = clock();
        execution_times[i] = ((double)(end - start)) / CLOCKS_PER_SEC;
    }

    // printf("Final Linked List:\n");
    // printLinkedList();

    mean = calculate_mean(execution_times, repetitions);   /*calculate the mean of the ran 100 execution*/
    stddev = calculate_standard_deviation(execution_times, repetitions, mean);   /*calculate standard deviation of the samples*/
    
    double z = 1.960; // 95% confidence level
    double error = 0.05 * mean; // desired accuracy in seconds
    
    required_samples = calculate_required_samples(stddev, mean, z, error);   /*calculate the required samples to get the desired confidence accuracy*/
    
    printf("Mean execution time: %f seconds\n", mean);
    printf("Standard deviation: %f seconds\n", stddev);
    printf("Required number of samples: %f\n", required_samples);
    printf("runnning the programme with the required number of samples...\n\n");

    double realExecutionTimes[(int)required_samples];
   
    for (int i = 0; i < required_samples; i++) {
        clock_t start = clock();
        
        for (int j = 0; j < m; j++) {
            int value = rand() % MAX_VALUE;
            switch (operations[j]) {
                case 'I':
                    insert(value);
                    break;
                case 'D':
                    delete(value);
                    break;
                case 'M':
                    member(value);
                    break;
            }
        }

        clock_t end = clock();
        realExecutionTimes[i] = ((double)(end - start)) / CLOCKS_PER_SEC;
    }

    mean = calculate_mean(realExecutionTimes, required_samples);   /*calculate the mean of the ran 100 execution*/
    stddev = calculate_standard_deviation(realExecutionTimes, required_samples, mean);   /*calculate standard deviation of the samples*/
    printf("Mean execution time: %f seconds, std: %f for samples: %d", mean,stddev, (int)required_samples);
    

    /*free the memory*/
    free(execution_times);
    free(realExecutionTimes);
    free(unique_values);

    return 0;


}

void generate_unique_values(int* values, int n) {
    bool used[MAX_VALUE] = {false};
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

int member(int value) {
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
    /*here for the simplicity of the execution we insert the nodes to the head of the linked list*/
    struct Node* new_node = malloc(sizeof(struct Node));
  
    new_node->data = value;
    new_node->next = head;
    head = new_node;

}

void delete(int value) {
   
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
