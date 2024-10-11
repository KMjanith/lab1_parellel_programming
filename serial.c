#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define MAX_VALUE 65536  // Range of random values
#define REPETITIONS 100  // Number of repetitions

struct Node {
    int data;
    struct Node* next;
};  

void generate_unique_values(int* values, int n);
void createInitialLinkedList(int* values, int n);
void createOperationList(char* operations, int m, float m_insert, float m_delete, float m_member);
struct Node* deleteList();
void shuffleOperations(char* operations, int m);
int member(int value);
void insert(int value);
void delete(int value);
double calculate_mean(double* values, int size);
double calculate_standard_deviation(double* values, int size, double mean);
double calculate_required_samples(double stddev, double mean, double z, double error);

struct Node* head = NULL;  /* Initial linked list */
int thread_count;          /* Number of threads from command line */
int n;                     /* Number of nodes in the linked list */
int m;                     /* Number of operations to perform */
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

int main(int argc, char* arg[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s <thread_count> <n> <m> <m_member> <m_insert> <m_delete>\n", arg[0]);
        return 1;
    }

    /* Get the number of threads from the command line */
    thread_count = strtol(arg[1], NULL, 10);
    n = strtol(arg[2], NULL, 10); /* Items to linked list */
    m = strtol(arg[3], NULL, 10); /* Number of operations to perform */
    m_member = strtof(arg[4], NULL); /* Number of member operations */
    m_insert = strtof(arg[5], NULL); /* Number of insert operations */
    m_delete = strtof(arg[6], NULL); /* Number of delete operations */

    //int repetitions = 100; // Initial number of repetitions
    double* execution_times = malloc(REPETITIONS * sizeof(double));  /* Store the execution times */
    if (execution_times == NULL) {
        perror("Failed to allocate memory for execution times");
        return 1;
    }

    double mean, stddev, required_samples;   /* Variables to calculate the stats */

    /* Generate unique values */
    int* unique_values = malloc(n * sizeof(int));
    if (unique_values == NULL) {
        perror("Failed to allocate memory for unique values");
        free(execution_times);
        return 1;
    }
    generate_unique_values(unique_values, n);

    /* Create initial linked list */
    createInitialLinkedList(unique_values, n);

    /* Dividing operations */
    char* operations = malloc(m * sizeof(char));
    if (operations == NULL) {
        perror("Failed to allocate memory for operations");
        free(execution_times);
        free(unique_values);
        return 1;
    }
    createOperationList(operations, m, m_insert, m_delete, m_member);

    /* Shuffling the operations */
    shuffleOperations(operations, m);

    for (int i = 0; i < REPETITIONS; i++) {
        if(i > 0 ){
            head = deleteList();
            createInitialLinkedList(unique_values, n);
        }
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

    mean = calculate_mean(execution_times, REPETITIONS);   /* Calculate the mean of the 100 executions */
    stddev = calculate_standard_deviation(execution_times, REPETITIONS, mean);   /* Calculate standard deviation of the samples */
    
    double z = 1.960; // 95% confidence level
    double error = 0.05 * mean; // Desired accuracy in seconds
    
    required_samples = calculate_required_samples(stddev, mean, z, error);   /* Calculate the required samples for desired confidence accuracy */
    

    double* realExecutionTimes = malloc((int)required_samples * sizeof(double));
    if (realExecutionTimes == NULL) {
        perror("Failed to allocate memory for real execution times");
        free(execution_times);
        free(unique_values);
        free(operations);
        return 1;
    }
   
    for (int i = 0; i < (int)required_samples; i++) {
        if(i > 0 ){
            head = deleteList();
            createInitialLinkedList(unique_values, n);
        }
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

    mean = calculate_mean(realExecutionTimes, (int)required_samples);   /* Calculate the mean of the executions */
    stddev = calculate_standard_deviation(realExecutionTimes, (int)required_samples, mean);   /* Calculate standard deviation of the samples */
    printf("Mean execution time: %f std: %f samples: %d\n\n", mean, stddev, (int)required_samples);
    
    /* Free the memory */
    free(execution_times);
    free(realExecutionTimes);
    free(unique_values);
    free(operations);

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

void createInitialLinkedList(int* values, int n) {
    for(int i = 0; i < n; i++) {
        insert(values[i]);
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

void createOperationList(char* operations, int m, float m_insert, float m_delete, float m_member) {
    int insert = (int)(m * m_insert);
    int delete = (int)(m * m_delete);
    int member = (int)(m * m_member);
    
    for(int i = 0; i < insert; i++) {
        operations[i] = 'I';
    }
    for(int i = insert; i < insert + delete; i++) {
        operations[i] = 'D';
    }
    for(int i = insert + delete; i < insert + delete + member; i++) {
        operations[i] = 'M';
    }
}

void shuffleOperations(char* operations, int m) {
    for(int i = 0; i < m; i++) {
        int j = rand() % m;
        char temp = operations[i];
        operations[i] = operations[j];
        operations[j] = temp;
    }
}

int member(int value) {
    struct Node* current = head;
    
    while (current != NULL && current->data < value) {
        current = current->next;
    }
    if (current == NULL || current->data > value) {
        return 0;
    }else{
        return 1;
    }
 
    return 0; /*value not found*/
    }

void insert(int value) {
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
