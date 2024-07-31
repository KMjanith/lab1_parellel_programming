#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define MAX_VALUE 65535

// Node structure for linked list
struct Node {
    int data;
    struct Node* next;
};

// Global variables
struct Node* head = NULL;
pthread_rwlock_t rwlock; // Read-write lock
pthread_t* threads; // Array to hold thread IDs
int* used; 
int total_operations;

// Insert a value into the linked list
void insert(struct Node** head, int value, pthread_rwlock_t* lock) {
    pthread_rwlock_wrlock(lock); 

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

    pthread_rwlock_unlock(lock); 
}

// Delete a node from the linked list
void deleteNode(struct Node** head_ref, int valueToDelete, pthread_rwlock_t* lock) {
    pthread_rwlock_wrlock(lock); 

    struct Node* current_node = *head_ref;
    struct Node* previous_node = NULL;

    while (current_node != NULL && current_node->data < valueToDelete) {
        previous_node = current_node;
        current_node = current_node->next;
    }

    if (current_node != NULL && current_node->data == valueToDelete) {
        if (previous_node == NULL) {
            *head_ref = current_node->next; // Delete head
        } else {
            previous_node->next = current_node->next; 
        }
        free(current_node);
    }

    pthread_rwlock_unlock(lock); 
}

// Check if a value exists in the linked list
int member(struct Node** head, int value) {
    struct Node* current_node = *head;
    while (current_node != NULL && current_node->data < value) {
        current_node = current_node->next;
    }

    return (current_node != NULL && current_node->data == value) ? 1 : 0;
}

// Generate a unique random number
int generate_unique_random() {
    int num;
    do {
        num = rand() % (MAX_VALUE + 1);
    } while (used[num]); 

    used[num] = 1; 
    return num;
}

// Function for each thread to perform operations
void* perform_operations(void* arg) {
    int total_operations_per_thread = *(int*)arg; 

    for (int i = 0; i < total_operations_per_thread; ++i) {
        int operation = rand() % 3; // Randomly choose an operation (0: insert, 1: delete, 2: member)
        int value;

        switch (operation) {
            case 0: // Insert operation
                value = generate_unique_random(); 
                insert(&head, value, &rwlock);
                break;

            case 1: // Delete operation
                value = rand() % (MAX_VALUE + 1); 
                if (member(&head, value)) {
                    deleteNode(&head, value, &rwlock);
                }
                break;

            case 2: // Member check operation
                value = rand() % (MAX_VALUE + 1); 
                member(&head, value);
                break;
        }
    }

    return NULL;
}

// Calculate the mean of an array
double calculate_mean(double* values, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += values[i];
    }
    return sum / size;
}

// Calculate the standard deviation of an array
double calculate_standard_deviation(double* values, int size, double mean) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += pow(values[i] - mean, 2);
    }
    return sqrt(sum / size);
}

// Calculate the required number of samples
double calculate_required_samples(double stddev, double mean, double z, double error) {
    return pow((z * stddev) / error, 2);
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s <n> <m> <m_member_fraction> <m_delete_fraction> <m_insert_fraction>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int number_of_threads = atoi(argv[1]); // Number of threads

    int n = atoi(argv[2]); // Number of threads
    total_operations = atoi(argv[3]); // Total number of operations for all threads
    float m_member_fraction = atof(argv[4]);
    float m_delete_fraction = atof(argv[5]);
    float m_insert_fraction = atof(argv[6]);

    // Validate input fractions
    if ((int)(m_member_fraction + m_delete_fraction + m_insert_fraction) != 1) {
        fprintf(stderr, "Fractions must sum to 1.0\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL)); 

    // Initialize the linked list with unique random values
    used = (int*)calloc(MAX_VALUE + 1, sizeof(int)); // Track used values
    threads = malloc(sizeof(pthread_t) * number_of_threads); // Allocate memory for thread IDs
    for (int i = 0; i < n; ++i) {
        insert(&head, generate_unique_random(), &rwlock);
    }

    // Initialize the read-write lock
    pthread_rwlock_init(&rwlock, NULL);

    int num_iterations = 100; // Number of times to run the operations
    double* execution_times = malloc(num_iterations * sizeof(double)); // Array to store execution times

    // Calculate the number of operations each thread will perform
    int total_operations_per_thread = total_operations / n;

    // Perform multiple iterations
    for (int iteration = 0; iteration < num_iterations; iteration++) {
        // Start measuring time
        clock_t start_time = clock();

        // Create threads for this iteration
        for (int i = 0; i < number_of_threads; ++i) {
            if (pthread_create(&threads[i], NULL, perform_operations, &total_operations_per_thread) != 0) {
                perror("Failed to create thread");
                return EXIT_FAILURE; // Handle thread creation error
            }
        }

        for (int i = 0; i < number_of_threads; ++i) {
            pthread_join(threads[i], NULL);
        }

        // End timing
        clock_t end_time = clock();
        execution_times[iteration] = (double)(end_time - start_time) / CLOCKS_PER_SEC; 
    }

    // Calculate mean execution time, standard deviation, and required samples
    double mean = calculate_mean(execution_times, num_iterations);
    double stddev = calculate_standard_deviation(execution_times, num_iterations, mean);
    double z = 1.960; // 95% confidence level
    double error = 0.05 * mean; // Desired accuracy in seconds
    double required_samples = calculate_required_samples(stddev, mean, z, error);

    // Print execution statistics
    // printf("Mean execution time: %f seconds\n", mean);
    // printf("Standard deviation: %f seconds\n", stddev);
    printf("Required number of samples: %f\n", required_samples);

    // Free the execution times array
    free(execution_times);

    // Allocate memory for realExecutionTimes dynamically
    double* realExecutionTimes = (double*)malloc((int)required_samples * sizeof(double));
    if (realExecutionTimes == NULL) {
        perror("Failed to allocate memory for realExecutionTimes");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < (int)required_samples; i++) {
        clock_t start = clock();
        
        // Create the threads
        for (int j = 0; j < number_of_threads; j++) {
            if (pthread_create(&threads[j], NULL, perform_operations, &total_operations_per_thread) != 0) {
                perror("Failed to create thread");
                return EXIT_FAILURE; // Handle thread creation error
            }
        }

        // Join the threads
        for (int j = 0; j < number_of_threads; j++) {
            pthread_join(threads[j], NULL);
        }

        clock_t end = clock();
        realExecutionTimes[i] = ((double)(end - start)) / CLOCKS_PER_SEC;
    }

    mean = calculate_mean(realExecutionTimes, (int)required_samples);
    stddev = calculate_standard_deviation(realExecutionTimes, (int)required_samples, mean);
    printf("Mean execution time: %f seconds, std: %f for samples: %d\n", mean, stddev, (int)required_samples);

    // Free the realExecutionTimes array
    free(realExecutionTimes);

    // Clean up resources after all iterations
    pthread_rwlock_destroy(&rwlock); 
    free(used); 
    free(threads); 
    while (head != NULL) { 
        struct Node* to_delete = head;
        head = head->next;
        free(to_delete);
    }

    return 0;
}
