/**
 *===================================================================================
 * @file           : random_alloc_free_test.c
 * @author         : Ali Mamdouh
 * @brief          : Random allocation and deallocation test for heap manager
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
 *===================================================================================
 * 
 *===================================================================================
 */
 
 
 
 

/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include <stdio.h>             // Include the standard I/O library for functions like printf for logging output.
#include <stdlib.h>            // Include the standard library for memory allocation and management functions.
#include <time.h>              // Include the time library for generating time-based random seeds.
#include "HMM.h"               // Include the custom heap manager's public API declarations.






/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/

/**
 * @brief Number of allocation slots available for memory blocks.
 * 
 * This macro defines the number of memory blocks that can be allocated and managed
 * simultaneously during the test. It represents the size of the array that holds
 * pointers to the allocated memory blocks.
 */
#define NUM_ALLOCS 10000

/**
 * @brief Maximum size of the memory blocks to be allocated.
 * 
 * This macro sets the upper limit on the size of the memory blocks (in bytes) that 
 * can be requested during the test. The actual size of each allocation is determined
 * randomly, ranging from 1 byte to this maximum value.
 */
#define MAX_SIZE 10500

/**
 * @brief Number of iterations for the random allocation and deallocation test.
 * 
 * This macro defines how many times the test will loop, performing random allocations
 * and deallocations of memory. Each iteration may either allocate a new block of memory
 * or free an existing block based on random selection.
 */
#define MAX_ITERATIONS 10000






/*============================================================================
 **************************  Variables Definitions  **************************
 ============================================================================*/

/**
 * @brief Tracks the last error code returned by the heap manager.
 * 
 * This variable holds the error code returned by the heap manager during the most recent
 * memory operation. It is initialized to 0 (indicating no error) and updated whenever 
 * an error occurs during allocation or deallocation.
 */
int Error_Code = 0;

/**
 * @brief Counter for the number of free operations performed.
 * 
 * This variable keeps track of how many times memory has been deallocated (freed)
 * during the test. It is incremented each time a free operation is performed to 
 * provide a count of deallocations across the test's iterations.
 */
int Free_Counter = 0;

/**
 * @brief Array to store values written to the allocated memory blocks.
 * 
 * This array stores the values that are written to the memory blocks during allocation.
 * Each index corresponds to a memory block managed by the test, and the value at each
 * index is used to verify that the data has not been corrupted when the memory is freed.
 */
int values_in_pointers[NUM_ALLOCS] = {0};






/**
 * @brief Conducts a random allocation and deallocation test on the heap manager.
 * 
 * This function performs a series of random memory allocations and deallocations
 * to test the functionality and robustness of the custom heap manager. The test
 * allocates memory for a random number of blocks, writes random values to the 
 * allocated memory, and then frees the memory in a random sequence. It also checks 
 * for memory corruption by verifying that the values written to memory are intact 
 * before deallocating the memory.
 *
 * The test iterates a predefined number of times (`MAX_ITERATIONS`) and logs each 
 * allocation and deallocation operation, including any errors encountered. If an 
 * error is detected during allocation, the function exits early. The test also 
 * ensures that all allocated memory is freed before the function returns.
 *
 * @return void
 */
void random_alloc_free_test() 
{
    // Seed the random number generator with the current time
    srand((unsigned int)time(NULL));
    
    // Array to hold pointers to allocated memory blocks
    void* pointers[NUM_ALLOCS] = {NULL};

    // Loop through the specified number of iterations
    for (int i = 0; i < MAX_ITERATIONS; ++i) 
    {
        //printf("Iteration Number: %i\n", i);
        int index = rand() % NUM_ALLOCS;

        // If the pointer at the selected index is NULL, allocate memory
        if (pointers[index] == NULL) 
        {
            // Allocate a random size of memory between 1 and MAX_SIZE
            size_t size = (size_t)(rand() % MAX_SIZE) + 1;

            // Attempt to allocate memory using the custom heap manager
            pointers[index] = HmmAlloc(size);
            if (pointers[index] != NULL) 
            {
                printf("Allocated memory of size %zu at address %p\n", size, pointers[index]);
                
                // Write a random value to the allocated memory
                int x = rand() % 16000; 
                *(int *)pointers[index] = x; 
                values_in_pointers[index] = x; // Store the value to verify later
                printf("Wrote memory value %d at address %p\n", x, pointers[index]);
                
                // Check and report any errors during allocation
                printf("Last Error Code is %d\n", hmm_get_last_error());
                if(hmm_get_last_error() != HMM_SUCCESS)
                {
                    Error_Code = hmm_get_last_error();
                    return;
                }

            } 
            else 
            {
                fprintf(stderr, "Allocation failed for size %zu\n", size);
                printf("Last Error Code is %d\n", hmm_get_last_error());
            }
            
        } 
        else 
        {
            // If the pointer is not NULL, free the allocated memory
            Free_Counter++;
            printf("Iteration Number: %i\n", i);
            printf("Number of free operations: %i\n", Free_Counter);
            
            printf("Freeing memory at address %p\n", pointers[index]);
            
            // Verify that the value in memory has not been corrupted
            if(*(int *)pointers[index] != values_in_pointers[index])
            {
                printf("Error: value at address %p is corrupted!!, original value = %d, corrupted value is %d\n", pointers[index], values_in_pointers[index], *(int*)pointers[index]);
                printf("Last Error Code is %d\n", hmm_get_last_error());
                print_free_list();
                return;
            }
            
            // Free the memory and set the pointer to NULL
            HmmFree(pointers[index]);
            pointers[index] = NULL;
        }
    }
    
    // Ensure all allocated memory is freed at the end of the test
    for (int i = 0; i < NUM_ALLOCS; ++i) 
    {
        //printf("Iteration Number in Free: %i\n", i);

        if (pointers[i] != NULL) 
        {
            Free_Counter++;
            printf("Freeing remaining memory at address %p\n", pointers[i]);
            printf("Number of free operations: %i\n", Free_Counter);
            
            // Verify that the value in memory has not been corrupted
            if(*(int *)pointers[i] != values_in_pointers[i])
            {
                printf("Error: value at address %p is corrupted!!, original value = %d, corrupted value is %d\n", pointers[i], values_in_pointers[i], *(int*)pointers[i]);
                print_free_list();
                return;
            }

            // Free the memory and set the pointer to NULL
            HmmFree(pointers[i]);
            pointers[i] = NULL;
        }
    }
}







/**
 * @brief Main function to execute the random allocation and deallocation test.
 *
 * This function serves as the entry point of the program. It first initializes
 * the heap manager by calling `hmm_init()`, which sets up the necessary data
 * structures and prepares the heap for allocation operations. After initialization,
 * it starts the random allocation and deallocation test by calling the 
 * `random_alloc_free_test()` function.
 *
 * During the test, various memory blocks are allocated and freed randomly, simulating
 * real-world usage patterns of dynamic memory allocation. The function logs the start
 * and end of the test to the console.
 *
 * @return Returns 0 to indicate successful completion of the program.
 */
int main() 
{
    // Initialize the heap manager
    hmm_init();
    
    // Log the start of the test
    printf("Starting random allocation and deallocation test...\n");
    
    // Perform the random allocation and deallocation test
    random_alloc_free_test();
    
    // Log the completion of the test
    printf("Test complete.\n");
    
    // Return success code
    return 0;
}

