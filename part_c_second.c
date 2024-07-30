#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_VALUE 65535 
#define NUM_OPERATIONS 1000 
#define INITIAL_LIST_SIZE 1000
#define NUM_THREADS 4

// Node structure for linked list
struct Node {
    int data;
    struct Node* next;
};

// Global variables
struct Node* head = NULL;
pthread_rwlock_t rwlock; // Read-write lock
pthread_t threads[NUM_THREADS]; // Array to hold thread IDs
int used[MAX_VALUE + 1] = {0}; // Track used values

// Insert a value into the linked list
void insert(struct Node** head, int value, pthread_rwlock_t* lock) {
    pthread_rwlock_wrlock(lock); // Acquire write lock

    struct Node* current_node = *head;
    struct Node* previous_node = NULL;
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = value;
    new_node->next = NULL;

    // Find the correct position to insert the new node
    while (current_node != NULL && current_node->data < value) {
        previous_node = current_node;
        current_node = current_node->next;
    }

    // Insert at the head
    if (previous_node == NULL) {
        new_node->next = *head;
        *head = new_node;
    } else {
        new_node->next = current_node;
        previous_node->next = new_node;
    }

    pthread_rwlock_unlock(lock); // Release write lock
}

// Delete a node from the linked list
void deleteNode(struct Node** head_ref, int valueToDelete, pthread_rwlock_t* lock) {
    pthread_rwlock_wrlock(lock); // Acquire write lock

    struct Node* current_node = *head_ref;
    struct Node* previous_node = NULL;

    while (current_node != NULL && current_node->data < valueToDelete) {
        previous_node = current_node;
        current_node = current_node->next;
    }

    // Node to delete found
    if (current_node != NULL && current_node->data == valueToDelete) {
        if (previous_node == NULL) {
            *head_ref = current_node->next; // Delete head
        } else {
            previous_node->next = current_node->next; // Bypass the current node
        }
        free(current_node);
        used[valueToDelete] = 0; // Mark the value as unused
    }

    pthread_rwlock_unlock(lock); // Release write lock
}

// Check if a value exists in the linked list
int member(struct Node** head, int value) {
    

    struct Node* current_node = *head;
    while (current_node != NULL && current_node->data < value) {
        current_node = current_node->next;
    }

    int result = (current_node != NULL && current_node->data == value) ? 1 : 0;

    
    return result;
}

// Generate a unique random number
int generate_unique_random() {
    int num;
    do {
        num = rand() % (MAX_VALUE + 1);
    } while (used[num]); // Repeat until a unique number is found

    used[num] = 1; // Mark this number as used
    return num;
}

// Function for each thread to perform operations
void* perform_operations(void* arg) {
    int total_operations = *(int*)arg;

    printf("Thread %ld starting with %d total operations.\n", pthread_self(), total_operations);

    for (int i = 0; i < total_operations; ++i) {
        int operation = rand() % 3; // Randomly choose an operation (0: insert, 1: delete, 2: member)
        int value;

        switch (operation) {
            case 0: // Insert operation
                value = generate_unique_random(); // Generate a unique random value
                insert(&head, value, &rwlock);
                printf("Thread %ld inserted %d\n", pthread_self(), value);
                break;

            case 1: // Delete operation
                value = rand() % (MAX_VALUE + 1); // Generate a random value
                if (member(&head, value)) {
                    deleteNode(&head, value, &rwlock);
                    printf("Thread %ld deleted %d\n", pthread_self(), value);
                } else {
                    printf("Thread %ld tried to delete %d, but it was not found.\n", pthread_self(), value);
                }
                break;

            case 2: // Member check operation
                value = rand() % (MAX_VALUE + 1); // Generate a random value
                if (member(&head, value)) {
                    printf("Thread %ld found value %d in list.\n", pthread_self(), value);
                } else {
                    printf("Thread %ld did not find value %d in list.\n", pthread_self(), value);
                }
                break;
        }
    }

    return NULL;
}

int main() {
    srand(time(NULL)); // Seed the random number generator

    // Initialize the linked list with unique random values
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

    // Determine the number of operations per thread
    int total_operations_per_thread = NUM_OPERATIONS / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("Creating thread %d\n", i);
        if (pthread_create(&threads[i], NULL, perform_operations, &total_operations_per_thread) != 0) {
            perror("Failed to create thread");
            return 1; // Handle thread creation error
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL); // Wait for all threads to finish
        printf("Joined thread %d\n", i); // Debug output
    }

    // End timing
    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC; // Convert to seconds

    // Clean up resources
    pthread_rwlock_destroy(&rwlock); // Destroy the read-write lock

    // Free the linked list memory
    while (head != NULL) {
        struct Node* to_delete = head;
        head = head->next;
        free(to_delete);
    }

    // Print execution time
    printf("Execution time: %f seconds\n", time_taken);

    return 0;
}
