#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_VALUE 65535 
#define NUM_OPERATIONS 1000 
#define INITIAL_LIST_SIZE 1000
#define NUM_THREADS 4

// Global variables
struct Node* head = NULL;
pthread_rwlock_t rwlock; // Read-write lock
pthread_t threads[NUM_THREADS]; // Array to hold thread IDs
int used[MAX_VALUE + 1] = {0}; // Track used values

// linked list struct and operations
struct Node {
    int data;
    struct Node* next;
};

void insert(struct Node** head, int value, pthread_rwlock_t* lock) {
    pthread_rwlock_wrlock(lock); // Acquire write lock

    struct Node* current_node = *head;
    struct Node* previous_node = NULL;
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = value;
    new_node->next = NULL;

   
    while (current_node != NULL && current_node->data < value) {
        previous_node = current_node;
        current_node = current_node->next;
    }

    
    if (previous_node == NULL) {
        new_node->next = *head;
        *head = new_node;
    } else {
        new_node->next = current_node;
        previous_node->next = new_node;
    }

    pthread_rwlock_unlock(lock); // Release write lock
}


void deleteNode(struct Node** head_ref, int valueToDelete, pthread_rwlock_t* lock) {
    pthread_rwlock_wrlock(lock); // Acquire write lock

    struct Node* current_node = *head_ref;
    struct Node* previous_node = NULL;

    while (current_node != NULL && current_node->data < valueToDelete) {
        previous_node = current_node;
        current_node = current_node->next;
    }

   
    if (current_node != NULL && current_node->data == valueToDelete) {
        if (previous_node == NULL) {
            *head_ref = current_node->next; 
        } else {
            previous_node->next = current_node->next; 
        }
        free(current_node);
        used[valueToDelete] = 0; // Mark the value as unused
    }

    pthread_rwlock_unlock(lock); // Release write lock
}


int member(struct Node** head, int value, pthread_rwlock_t* lock) {
    pthread_rwlock_rdlock(lock); // Acquire read lock

    struct Node* current_node = *head;
    while (current_node != NULL && current_node->data < value) {
        current_node = current_node->next;
    }

    int result = (current_node != NULL && current_node->data == value) ? 1 : 0;

    pthread_rwlock_unlock(lock); // Release read lock
    return result;
}

// Generate a unique random number
int generate_unique_random() {
    int num;
    do {
        num = rand() % (MAX_VALUE + 1);
    } while (used[num]); 

    used[num] = 1; // Mark this number as used
    return num;
}

// Function for each thread to perform operations
void* perform_operations(void* arg) {
    int* operation_counts = (int*)arg;
    int insert_count = operation_counts[0];
    int delete_count = operation_counts[1];
    int member_count = operation_counts[2];

   
    printf("Thread %ld starting with %d inserts, %d deletes, %d member checks.\n", 
           pthread_self(), insert_count, delete_count, member_count);

    for (int i = 0; i < insert_count; ++i) {
        int value = generate_unique_random(); // Generate a unique random value
        insert(&head, value, &rwlock);
        printf("Thread %ld inserted %d\n", pthread_self(), value);
    }

    for (int i = 0; i < delete_count; ++i) {
        int value = rand() % (MAX_VALUE + 1); 
        if (member(&head, value, &rwlock)) {
            deleteNode(&head, value, &rwlock);
            printf("Thread %ld deleted %d\n", pthread_self(), value);
        } else {
            printf("Thread %ld tried to delete %d, but it was not found.\n", pthread_self(), value);
        }
    }

    for (int i = 0; i < member_count; ++i) {
        int value = rand() % (MAX_VALUE + 1); 
        int result = member(&head, value, &rwlock);
        if (result) {
            printf("Thread %ld found value %d in list.\n", pthread_self(), value);
        } else {
            printf("Thread %ld did not find value %d in list.\n", pthread_self(), value);
        }
    }

    return NULL;
}

int main() {
    srand(time(NULL)); 

    // linkedlist with random varibales 
    printf("Starting serial implementation:\n");
    for (int i = 0; i < INITIAL_LIST_SIZE; ++i) {
        int value = generate_unique_random();
        insert(&head, value, &rwlock);
    }

    printf("Initial linked list:\n");
    struct Node* temp = head;
    while (temp != NULL) {
        printf("%d\n", temp->data);
        temp = temp->next;
    }

    printf("\n");

    // Initialize the read-write lock
    pthread_rwlock_init(&rwlock, NULL);
    printf("Starting parallel implementation with read-write locks:\n");

    // Start measuring time
    clock_t start_time = clock();

    int NUM_OPERATIONS_Per_Thread = NUM_OPERATIONS / NUM_THREADS;

    // Prepare operation counts for each thread
    int operation_counts[3] = {NUM_OPERATIONS_Per_Thread / 3, NUM_OPERATIONS_Per_Thread / 3, NUM_OPERATIONS_Per_Thread / 3}; // m values

    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("Creating thread %d\n", i);
        if (pthread_create(&threads[i], NULL, perform_operations, operation_counts) != 0) {
            perror("Failed to create thread");
            return 1; 
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL); // Wait for all threads to finish
        printf("Joined thread %d\n", i); 
    }

    //end time
    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC; // Convert to seconds

   

    //final list
    // printf("Final linked list:\n");
    // temp = head;
    // while (temp != NULL) {
    //     printf("%d\n", temp->data);
    //     temp = temp->next;
    // }

    // Clean up resources
    pthread_rwlock_destroy(&rwlock); // Destroy the read-write lock

    // Free the linked list memory
    while (head != NULL) {
        struct Node* to_delete = head;
        head = head->next;
        free(to_delete);
    }

    //print thread execution time 
    printf("Execution time: %f seconds\n", time_taken);

    return 0;
}
