/**
 *===================================================================================
 * @file           : HMM.h
 * @author         : Ali Mamdouh
 * @brief          : Header interface of Heap manager
 * @Reviewer       : Eng Reda
 * @Version        : 2.1.0
 *===================================================================================
 * 
 *===================================================================================
 */





#ifndef HMM_H
#define HMM_H

/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include <stddef.h> 
#include <stdbool.h> 





/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/
/**
 * Defines the size by which the heap is expanded when additional memory is needed.
 *
 * This macro specifies the amount of memory (in bytes) that the heap will be increased
 * by each time a heap expansion is required. This ensures that the heap grows in large chunks to reduce the frequency of expansion
 * operations and improve memory management efficiency.
 *
 * A larger expansion size can help to minimize the overhead associated with frequent
 * heap expansions and reduce fragmentation by allocating a substantial block of memory
 * at once.
 */
#define HEAP_EXPAND_SIZE   (1024 * 1024)  // 1MB expansion size

// Specifies the minimum allocation size that the heap manager will handle.
// Any request for memory allocation smaller than this will be rounded up to this size.
// The reason is to reduce overhead and EXTERNAL fragmentation as possible
#define MIN_ALLOC_SIZE     40 // 64-bit processor, is 32 to take care of metadata

// Defines the alignment requirement for memory blocks allocated by the heap manager.
// For Example: 10 bytes would be rounded up to size of 16 bytes (since 16 is the nearest multiple of 8 greater than 10)-
// The additional 6 bytes are not wasted; they are a trade-off to ensure that the next allocation also starts at an aligned address.
#define ALIGNMENT          8 // 64-bit processor








/*============================================================================
 *************************  Data types Declerations  *************************
 ============================================================================*/
// Error codes
typedef enum 
{
    HMM_SUCCESS,                      // Operation completed successfully.
    HMM_ERROR_OUT_OF_MEMORY,          // Memory allocation failed due to insufficient memory.
    HMM_ERROR_INVALID_POINTER,        // Invalid pointer provided for deallocation or access.
    HMM_ERROR_DOUBLE_FREE             // Attempted to free a memory block that has already been freed.
} hmm_error_t;




// Allocation algorithms
typedef enum 
{
    FIRST_FIT,  // Allocate the first free block that fits the requested size.
    BEST_FIT,   // Allocate the smallest free block that fits the requested size.
    WORST_FIT   // Allocate the largest free block available.
} hmm_alloc_algorithm_t;



/*============================================================================
 *************************  Functions API Decleration  ***********************
 ============================================================================*/
// Public API
void* HmmAlloc(size_t size);
void HmmFree(void* ptr);
hmm_error_t hmm_get_last_error(void);
void hmm_set_allocation_algorithm(hmm_alloc_algorithm_t algorithm);
void hmm_init(void);
void hmm_cleanup(void);

// Standard memory function declarations
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

// Debug Functions
void print_free_list(void);

#endif // HMM_H

