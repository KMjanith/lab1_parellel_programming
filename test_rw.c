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
    pthread_rwlock_t lock; // Add a read-write lock for each node
};

// Structure to hold parameters for threads
struct ThreadParams {
    int total_operations_per_thread;
    float m_member_fraction;
    float m_delete_fraction;
    float m_insert_fraction;
    struct Node** head; // Pass the head of the linked list
    int* used; // Track used values
    pthread_rwlock_t* used_lock; // Lock for used array
    pthread_rwlock_t* head_lock; // Lock for head of the list
};

// Global variables for thread management
pthread_rwlock_t head_lock; // Lock for the head of the list
pthread_rwlock_t used_lock; // Lock for used array

// Insert a value into the linked list
void insert(struct Node** head, int value, pthread_rwlock_t* head_lock) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    if (!new_node) {
        perror("Failed to allocate memory for new node");
        return;
    }
    new_node->data = value;
    new_node->next = NULL;
    pthread_rwlock_init(&new_node->lock, NULL); // Initialize the node's lock

    pthread_rwlock_wrlock(head_lock); // Lock the head for writing

    struct Node* current_node = *head;
    struct Node* previous_node = NULL;

    // Find the position to insert the new node
    while (current_node != NULL && current_node->data < value) {
        previous_node = current_node;
        current_node = current_node->next;
    }

    // Insert the new node
    if (previous_node == NULL) {
        new_node->next = *head;
        *head = new_node;
    } else {
        new_node->next = current_node;
        previous_node->next = new_node;
    }

    pthread_rwlock_unlock(head_lock); // Unlock the head after insertion
}

// Delete a node from the linked list
void deleteNode(struct Node** head_ref, int valueToDelete, pthread_rwlock_t* head_lock) {
    pthread_rwlock_wrlock(head_lock); // Lock the head for writing
    struct Node* current_node = *head_ref;
    struct Node* previous_node = NULL;

    // Find the node to delete
    while (current_node != NULL && current_node->data < valueToDelete) {
        previous_node = current_node;
        current_node = current_node->next;
    }

    // If found, delete the node
    if (current_node != NULL && current_node->data == valueToDelete) {
        if (previous_node == NULL) {
            *head_ref = current_node->next; // Delete head
        } else {
            previous_node->next = current_node->next; 
        }
        
        pthread_rwlock_destroy(&current_node->lock); // Destroy the node's lock
        free(current_node);
    }
    
    pthread_rwlock_unlock(head_lock); // Unlock the head after deletion
}

// Check if a value exists in the linked list
int member(struct Node** head, int value, pthread_rwlock_t* head_lock) {
    pthread_rwlock_rdlock(head_lock); // Lock the head for reading
    struct Node* current_node = *head;

    while (current_node != NULL) {
        if (current_node->data == value) {
            pthread_rwlock_unlock(head_lock); // Unlock immediately after reading
            return 1; // Found
        }
        current_node = current_node->next;
    }

    pthread_rwlock_unlock(head_lock); // Unlock if not found
    return 0; // Not found
}

// Generate a unique random number
int generate_unique_random(int* used, pthread_rwlock_t* used_lock) {
    int num;
    do {
        num = rand() % (MAX_VALUE + 1);
        pthread_rwlock_rdlock(used_lock); // Lock the used array for reading
        if (!used[num]) {
            used[num] = 1;
            pthread_rwlock_unlock(used_lock); // Unlock immediately after using
            return num;
        }
        pthread_rwlock_unlock(used_lock); // Unlock if not found
    } while (1); // Loop until a unique number is found
}

// Function for each thread to perform operations
void* perform_operations(void* arg) {
    struct ThreadParams* params = (struct ThreadParams*)arg;

    int total_operations_per_thread = params->total_operations_per_thread; 
    float m_member_fraction = params->m_member_fraction; 
    float m_delete_fraction = params->m_delete_fraction; 
    float m_insert_fraction = params->m_insert_fraction; 

    int member_operations = (int)(total_operations_per_thread * m_member_fraction); 
    int delete_operations = (int)(total_operations_per_thread * m_delete_fraction); 
    int insert_operations = (int)(total_operations_per_thread * m_insert_fraction);

    int total_operation_to_cover = member_operations + delete_operations + insert_operations;

    while (0 <  total_operation_to_cover) {
        int operation = rand() % 3; // Randomly choose an operation (0: insert, 1: delete, 2: member)
        int value;

        switch (operation) {
            case 0: // Insert operation
                if (0 < insert_operations) {
                    value = generate_unique_random(params->used, params->used_lock); 
                    if (!member(params->head, value, params->head_lock)) {
                        insert(params->head, value, params->head_lock);
                        insert_operations--;
                    }
                }
                break;

            case 1: // Delete operation
                if (0 < delete_operations) {
                    value = rand() % (MAX_VALUE + 1); // Changed to random value
                    if (member(params->head, value, params->head_lock)) {
                        deleteNode(params->head, value, params->head_lock);
                        delete_operations--;
                    }
                }
                break;

            case 2: // Member check operation
                if (0 < member_operations) {
                    value = rand() % (MAX_VALUE + 1); // Changed to random value
                    member(params->head, value, params->head_lock);
                    member_operations--;
                }
                break;
        }

        total_operation_to_cover = member_operations + delete_operations + insert_operations;
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
    int n = atoi(argv[2]); // Number of initial inserts
    int total_operations = atoi(argv[3]); // Total number of operations for all threads
    float m_member_fraction = atof(argv[4]);
    float m_delete_fraction = atof(argv[5]);
    float m_insert_fraction = atof(argv[6]);

    // Validate input fractions
    if ((int)(m_member_fraction + m_delete_fraction + m_insert_fraction) != 1) {
        fprintf(stderr, "Fractions must sum to 1.0\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL)); 

    int num_iterations = 100; // Number of times to run the operations
    double* execution_times = malloc(num_iterations * sizeof(double)); // Array to store execution times

    // Calculate the number of operations each thread will perform
    int total_operations_per_thread = total_operations / number_of_threads;

    // Perform multiple iterations
    for (int iteration = 0; iteration < num_iterations; iteration++) {
        struct Node* head = NULL;
        pthread_t* threads = malloc(sizeof(pthread_t) * number_of_threads); // Array to hold thread IDs
        int* used = (int*)calloc(MAX_VALUE + 1, sizeof(int)); // Track used values
        pthread_rwlock_init(&used_lock, NULL); // Initialize the used lock
        pthread_rwlock_init(&head_lock, NULL); // Initialize the head lock

        // Initialize the linked list with unique random values
        for (int i = 0; i < n; ++i) {
            insert(&head, generate_unique_random(used, &used_lock), &head_lock);
        }

        // Start measuring time
        clock_t start_time = clock();

        // Create threads for this iteration
        struct ThreadParams params = { total_operations_per_thread, m_member_fraction, m_delete_fraction, m_insert_fraction, &head, used, &used_lock, &head_lock };
        for (int i = 0; i < number_of_threads; ++i) {
            if (pthread_create(&threads[i], NULL, perform_operations, &params) != 0) {
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

        // Free resources after each test
        free(threads);
        free(used);
        
        // Clean up the linked list
        while (head != NULL) {
            struct Node* temp = head;
            head = head->next;
            free(temp);
        }
        pthread_rwlock_destroy(&head_lock);
        pthread_rwlock_destroy(&used_lock);
    }

    // Calculate mean execution time, standard deviation, and required samples
    double mean = calculate_mean(execution_times, num_iterations);
    double stddev = calculate_standard_deviation(execution_times, num_iterations, mean);
    double z = 1.960; // 95% confidence level
    double error = 0.05 * mean; // Desired accuracy in seconds
    double required_samples = calculate_required_samples(stddev, mean, z, error);

    // Print execution statistics
    printf("Required number of samples: %f\n", required_samples);

    // Free the execution times array
    free(execution_times);

    // Allocate memory for realExecutionTimes dynamically
    double* realExecutionTimes = (double*)malloc((int)required_samples * sizeof(double));
    if (realExecutionTimes == NULL) {
        perror("Failed to allocate memory for realExecutionTimes");
        return EXIT_FAILURE; // Handle memory allocation error
    }

    // Collect execution times for the required number of samples
    for (int iteration = 0; iteration < (int)required_samples; iteration++) {
        struct Node* head = NULL;
        pthread_t* threads = malloc(sizeof(pthread_t) * number_of_threads); // Array to hold thread IDs
        int* used = (int*)calloc(MAX_VALUE + 1, sizeof(int)); // Track used values
        pthread_rwlock_init(&used_lock, NULL); // Initialize the used lock
        pthread_rwlock_init(&head_lock, NULL); // Initialize the head lock
        
        // Initialize the linked list with unique random values
        for (int i = 0; i < n; ++i) {
            insert(&head, generate_unique_random(used, &used_lock), &head_lock);
        }

        clock_t start_time = clock();
        printf("Processing sample num %d\n", iteration);
        
        struct ThreadParams params = { total_operations_per_thread, m_member_fraction, m_delete_fraction, m_insert_fraction, &head, used, &used_lock, &head_lock };
        for (int i = 0; i < number_of_threads; ++i) {
            if (pthread_create(&threads[i], NULL, perform_operations, &params) != 0) {
                perror("Failed to create thread");
                return EXIT_FAILURE; // Handle thread creation error
            }
        }

        for (int i = 0; i < number_of_threads; ++i) {
            pthread_join(threads[i], NULL);
        }

        clock_t end_time = clock();
        realExecutionTimes[iteration] = (double)(end_time - start_time) / CLOCKS_PER_SEC; 

        // Free resources after each test
        free(threads);
        free(used);
        
        // Clean up the linked list
        while (head != NULL) {
            struct Node* temp = head;
            head = head->next;
            free(temp);
        }
        pthread_rwlock_destroy(&head_lock);
        pthread_rwlock_destroy(&used_lock);
    }

    // Calculate and print the final statistics
    mean = calculate_mean(realExecutionTimes, (int)required_samples);
    stddev = calculate_standard_deviation(realExecutionTimes, (int)required_samples, mean);
    printf("Final mean execution time: %f seconds\n", mean);
    printf("Final standard deviation of execution time: %f seconds\n", stddev);

    // Clean up
    free(realExecutionTimes);

    return EXIT_SUCCESS;
}
