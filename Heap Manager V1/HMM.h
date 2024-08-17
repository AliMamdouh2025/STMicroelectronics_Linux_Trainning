/**
 *===================================================================================
 * @file           : HMM.h
 * @author         : Ali Mamdouh
 * @brief          : Header interface of Heap manager
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
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
// Defines the total size of the heap memory managed by the heap manager.
// 1048576 bytes (1 MB) is a typical size used for testing or small-scale applications.
#define HEAP_SIZE          1048576  // 1 MB

// Specifies the minimum allocation size that the heap manager will handle.
// Any request for memory allocation smaller than this will be rounded up to this size.
// The reason is to reduce overhead and EXTERNAL fragmentation as possible
#define MIN_ALLOC_SIZE     8 // 64-bit processor

// Defines the alignment requirement for memory blocks allocated by the heap manager.
// For Example: 10 bytes would be rounded up to size of 16 bytes (since 16 is the nearest multiple of 8 greater than 10)-
// The additional 6 bytes are not wasted; they are a trade-off to ensure that the next allocation also starts at an aligned address.
#define ALIGNMENT          8 // 64-bit processor

/** This ensures that no single allocation exceeds a manageable size, 
 *  which could prevent over-committing memory or could be used for optimizing memory management algorithms.
 *
 * It may also serve as a safeguard to ensure that any requests for memory allocation exceeding SIZE_MAX
 * are either handled differently or flagged as an error.
 */
#define SIZE_MAX                  10000






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

#endif // HMM_H

