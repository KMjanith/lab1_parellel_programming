#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // Add this line to include the bool data type
#include <pthread.h>

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
void createOperationList(char* operations,int m, float m_insert,float m_delete,float m_member);
void shuffleOperations(char* operations, int m);
void *threadOperation(void* thread_data);
int member(int value);
void insert(int value);
void delete(int value);

struct Node* head = NULL;  /*making the initial linked list*/
int thread_count;        /*variable to store the thread count from the command line*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  /*mutex variable*/
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
    pthread_t *thread_list; /*array pointer of pthreads*/

    /*get the number of threads from the command line*/
    thread_count = strtol(arg[1], NULL, 10);
    n = strtol(arg[2], NULL, 10); /*items to linked list*/
    m = strtol(arg[3], NULL, 10); /*number of operations to perform*/
    m_member = strtof(arg[4], NULL); /*number of member operations*/
    m_insert = strtof(arg[5], NULL); /*number of insert operations*/
    m_delete = strtof(arg[6], NULL); /*number of delete operations*/
   

    /*generate unique values*/
    int* unique_values = malloc(n * sizeof(int));
    generate_unique_values(unique_values, n);
    // for(int i =0;i<n;i++){
    //     printf("%d\n", unique_values[i]);
    // }

    /*creating initial linked list*/
    createInitialLinkedList(unique_values,n);
    printf("Initial Linked List:\n");
    printLinkedList();

    //printf("head data %d\n", head->data);

    /*dividing operations*/
    char operations[m];
    createOperationList(operations, m, m_insert, m_delete, m_member);
    // for(int i = 0; i < m; i++){
    //     printf("%c\n", operations[i]);
    // }

    /*shuffling the operations*/
    shuffleOperations(operations, m);
    // for(int i = 0; i < m; i++){
    //     printf("%c\n", operations[i]);
    // }

    /*allocate memory for the array of pthreads*/
    thread_list = malloc(thread_count * sizeof(pthread_t));
    /*allocate memory for ThreadDataList*/
    struct ThreadData* thread_data = malloc(thread_count * sizeof(struct ThreadData));

    /*initialize the mutex*/
    if(pthread_mutex_init(&mutex, NULL) != 0){
        fprintf(stderr, "Error initializing mutex\n");
        exit(EXIT_FAILURE);
    }

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

    printf("Final Linked List:\n");
    printLinkedList();

    /*free the memory*/
    free(thread_list);
    free(thread_data);
    free(unique_values);

    /*destroy the mutex*/
    pthread_mutex_destroy(&mutex);

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
            member(random_value);
        }else if(operations[i] == 'I'){
            insert(random_value);
        }else{
            delete(random_value);
        }

    }

}

int member(int value) {
    pthread_mutex_lock(&mutex);
    struct Node* current = head;
    printf("Thread %ld: Member %d\n", pthread_self(), value);

    while (current != NULL) {
        if (current->data == value) {
            pthread_mutex_unlock(&mutex);
            return 1;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&mutex);
    return 0;
}


void insert(int value){
    /*here for the simplicity of the execution we insert the nodes to the head of the linked list*/
    struct Node* new_node = malloc(sizeof(struct Node));
    pthread_mutex_lock(&mutex);
    printf("Thread %ld: Insert %d\n", pthread_self(), value);
    new_node->data = value;
    new_node->next = head;
    head = new_node;
    pthread_mutex_unlock(&mutex);
}

void delete(int value) {
    pthread_mutex_lock(&mutex);
    struct Node* current = head;
    printf("Thread %ld: Delete %d\n", pthread_self(), value);
    if (current == NULL) { // Empty list
        pthread_mutex_unlock(&mutex);
        return;
    }

    if (current->data == value) { // Delete the head
        head = current->next;
        free(current);
        pthread_mutex_unlock(&mutex);
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
        pthread_mutex_unlock(&mutex);
        return;
    }

    previous->next = current->next;  // Delete the node
    free(current);

    pthread_mutex_unlock(&mutex);

}
