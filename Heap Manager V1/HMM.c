/**
 *===================================================================================
 * @file           : HMM.c
 * @author         : Ali Mamdouh
 * @brief          : Source of heap manager
 * @Reviewer       : Eng Reda
 * @Version        : 2.1.0
 *===================================================================================
 * 
 *===================================================================================
 */





/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include "HMM.h"              // Include the public header file for the heap manager, which contains the API declarations.
#include "HMM_internal.h"      // Include the internal header file for the heap manager, which contains internal data structures and helper function declarations.
#include <string.h>            // Include the string library to use functions like `memset`, which is used in the project to initialize memory.
#include <assert.h>            // Include the assert library to enable the use of the `assert` macro, which helps in debugging by checking assumptions made in the code.
#include <stdio.h>             // Include the standard input-output library to use functions like `printf` for debugging and displaying messages to the console.
#include <unistd.h>            // Include it for system calls





/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/





/*============================================================================
 **************************  Variables Definitions  **************************
 ============================================================================*/
// This pointer holds the address of the beginning of the heap.
// It is initially set to NULL and is assigned a value once the heap is initialized.
static void* heap_start = NULL;

// This pointer holds the address of the end of the heap.
// It is initially set to NULL and is assigned a value once the heap is initialized.
static void* heap_end = NULL;



// Pointer to the head of the free list, which keeps track of free memory blocks in the heap.
// Initially set to NULL, meaning no free blocks are available until the heap is initialized.
static block_metadata_t* free_list_head = NULL;

// Pointer to the current program break, representing the end of the allocated space in the heap.
// Initially starts at the beginning of the heap array.
//static char* program_break = heap;

// Variable to store the last error that occurred during memory allocation or deallocation.
// Initially set to HMM_SUCCESS, indicating no errors have occurred.
static hmm_error_t last_error = HMM_SUCCESS;

// Variable to store the current memory allocation algorithm being used by the heap manager.
// By default, it's set to FIRST_FIT.
static hmm_alloc_algorithm_t current_algorithm = FIRST_FIT;




/*============================================================================
 ***************************  Functions  Definition  *************************
 ============================================================================*/
/**
 * Increments the program's data space (heap) by the specified amount.
 *
 * This function requests an increase in the heap's size by the amount specified
 * by the `increment` parameter using the `sbrk` system call. If the request fails,
 * it sets an error flag and returns NULL. If successful, it returns a pointer to
 * the beginning of the newly allocated memory.
 *
 * The function handles error conditions by setting the `last_error` variable to
 * `HMM_ERROR_OUT_OF_MEMORY` when the `sbrk` call fails, indicating that there was
 * not enough memory available to fulfill the request.
 *
 * @param increment The amount by which to increase the heap. This value can be positive
 *                  (to expand the heap) or negative (to shrink the heap).
 *
 * @return A pointer to the start of the newly allocated memory if successful, or
 *         NULL if the `sbrk` call fails.
 */
static void* hmm_sbrk(intptr_t increment) 
{
    // Request a change in the heap size using the sbrk system call
    void* result = sbrk(increment);

    // Check if the sbrk call failed
    if (result == (void*)-1) 
    {
        // Set error flag indicating out of memory condition
        last_error = HMM_ERROR_OUT_OF_MEMORY;
        return NULL;
    }

    // Return the pointer to the newly allocated memory
    return result;
}








/**
 * Initializes the heap memory for the heap memory manager.
 *
 * This function sets up the initial state of the heap by expanding it and
 * initializing the first free block. The heap is expanded by the size defined
 * by `HEAP_EXPAND_SIZE` using the `hmm_sbrk` function. The `heap_start` pointer
 * is set to the beginning of the heap, and the `heap_end` pointer is set to the
 * end of the heap.
 *
 * The function also initializes the first free block within the heap. This block
 * spans the entire allocated heap space minus the size of the metadata. The block's
 * metadata fields are set to indicate the block is free, and a magic number is
 * assigned for integrity verification. The free list is updated to include this
 * initial block.
 *
 * Note: This function should be called only once to initialize the heap. If the heap
 *       has already been initialized, the function does nothing.
 */
void hmm_init(void) 
{
    // Check if the heap has not been initialized yet
    if (!heap_start) 
    {
        // Expand the heap by HEAP_EXPAND_SIZE and set heap_start
        heap_start = hmm_sbrk(HEAP_EXPAND_SIZE);

        // Check if heap expansion failed
        if (!heap_start) return;

        // Set the end of the heap based on the start address and expansion size
        heap_end = (char*)heap_start + HEAP_EXPAND_SIZE;
        
        // Initialize the first free block in the heap
        block_metadata_t* first_block = (block_metadata_t*)heap_start;

        // Set the size of the block and mark it as free
        first_block->size_and_flags = (HEAP_EXPAND_SIZE - sizeof(block_metadata_t)) | IS_FREE_MASK;

        // Set the magic number for integrity verification
        first_block->magic = MAGIC_NUMBER;

        // Initialize the pointers for the free block (no previous or next block yet)
        first_block->prev = NULL;
        first_block->next = NULL;

        // Update the free list to include this initial block
        free_list_head = first_block;
    }
}











/**
 * Expands the heap by a specified size to accommodate new memory allocations.
 *
 * This function calculates the size needed to expand the heap, ensuring it is
 * aligned to a multiple of `ALIGNMENT` and at least as large as `HEAP_EXPAND_SIZE`.
 * It then requests this size using the `hmm_sbrk` function. If successful, it
 * initializes a new free block with the allocated memory and updates the heap's
 * end pointer. The new block is inserted into the free list to be available for
 * future allocations.
 *
 * @param size The size of the memory needed. The heap will be expanded to accommodate
 *             this size, rounded up to the nearest alignment boundary.
 *
 * @return A pointer to the newly allocated block of memory if successful, or
 *         NULL if the heap expansion fails.
 */
static void* hmm_expand_heap(size_t size) 
{
    // Calculate the size to expand the heap to, including alignment and block metadata
    size_t expand_size = ((size + sizeof(block_metadata_t) + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;

    // Ensure the expansion size is at least as large as HEAP_EXPAND_SIZE
    if (expand_size < HEAP_EXPAND_SIZE) expand_size = HEAP_EXPAND_SIZE;
    
    // Request the memory expansion using hmm_sbrk
    void* new_mem = hmm_sbrk(expand_size);

    // Check if the heap expansion request failed
    if (!new_mem) return NULL;
    
    // Initialize a new free block with the newly allocated memory
    block_metadata_t* new_block = (block_metadata_t*)new_mem;

    // Set the size of the new block and mark it as free
    new_block->size_and_flags = (expand_size - sizeof(block_metadata_t)) | IS_FREE_MASK;

    // Set the magic number for integrity verification
    new_block->magic = MAGIC_NUMBER;

    // Initialize the pointers for the new block
    new_block->prev = NULL;
    new_block->next = free_list_head;

    // Update the previous head of the free list to point back to the new block
    if (free_list_head) free_list_head->prev = new_block;

    // Update the free list head to the new block
    free_list_head = new_block;
    
    // Update the end of the heap to reflect the newly allocated memory
    heap_end = (char*)new_mem + expand_size;

    // Return the new block to the caller
    return new_block;
}







/**
 * Removes a memory block from the free list.
 *
 * This function detaches a specified memory block from the free list by updating
 * the pointers of the adjacent blocks. The block is effectively removed from the
 * free list and its `prev` and `next` pointers are set to `NULL`.
 *
 * The removal process includes the following steps:
 * 1. Check if the block has a previous block in the free list:
 *    - If it does, update the `next` pointer of the previous block to point to the
 *      block's next block.
 *    - If it doesn't, update the `free_list_head` to point to the block's next block,
 *      indicating that the block being removed was the head of the free list.
 * 2. Check if the block has a next block in the free list:
 *    - If it does, update the `prev` pointer of the next block to point to the block's
 *      previous block.
 * 3. Set the `prev` and `next` pointers of the block being removed to `NULL` to
 *    completely detach it from the free list.
 *
 * This function is typically used internally by the heap memory manager when
 * a block is allocated or otherwise needs to be removed from the free list.
 *
 * @param block A pointer to the block metadata that is to be removed from the free list.
 */
void hmm_remove_from_free_list(block_metadata_t* block) 
{
    // If the block has a previous block, update the previous block's next pointer
    if (block->prev)
        block->prev->next = block->next;
    else
        // If the block is the head of the free list, update the free_list_head
        free_list_head = block->next;

    // If the block has a next block, update the next block's previous pointer
    if (block->next)
        block->next->prev = block->prev;

    // Detach the block by setting its previous and next pointers to NULL
    block->prev = NULL;
    block->next = NULL;
}


















/**
 * Allocates a block of memory from the heap.
 *
 * This internal function attempts to allocate a block of memory of a specified
 * size from the heap. It ensures that the allocated memory is aligned and large
 * enough to meet the minimum allocation size requirement. If a suitable block
 * is found in the free list, it may be split to fit the requested size. If no
 * suitable block is found, new memory is allocated by extending the program break.
 *
 * The allocation process includes the following steps:
 * 1. Align the requested size to ensure it meets the alignment requirement. If the
 *    size is smaller than the minimum allocation size, it is adjusted to the minimum.
 * 2. Attempt to find a suitable free block by calling `hmm_find_free_block`. If a
 *    suitable block is found, it may be split to fit the requested size.
 * 3. If no suitable free block is found, check if there is enough space left in the
 *    heap to allocate a new block. If there is enough space:
 *    - Create a new block at the current program break.
 *    - Initialize the block's metadata, including its size and a magic number for
 *      integrity checking.
 *    - Move the program break pointer forward by the size of the allocated block
 *      plus the size of its metadata.
 *    - If there is not enough space, return `NULL` to indicate an out-of-memory condition.
 * 4. Mark the block as allocated by clearing the allocation flag in its size field.
 * 5. Remove the block from the free list using `hmm_remove_from_free_list`.
 * 6. Return a pointer to the memory portion of the block (i.e., the address immediately
 *    after the block's metadata).
 *
 * This function is used internally by the heap memory manager to allocate memory and is
 * not intended to be called directly by user code.
 *
 * @param size The number of bytes to allocate, before alignment adjustments.
 * @return A pointer to the allocated memory if successful; otherwise, `NULL` if the
 *         allocation fails (e.g., due to insufficient memory).
 */
void* hmm_internal_alloc(size_t size) 
{
    if(size == 0) return NULL; 

    size = ((size + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;

    if (size < MIN_ALLOC_SIZE) size = MIN_ALLOC_SIZE;
    
    block_metadata_t* block = hmm_find_free_block(size);

    if (!block) 
    {
        block = hmm_expand_heap(size);
        if (!block) return NULL;
    }
    
    hmm_split_block(block, size);

    
    return (void*)(block + 1); // which is equivelent to ((block_metadata_t*)block+1), So user write to its dedicated space
}







/**
 * Frees a previously allocated block of memory.
 *
 * This internal function releases a block of memory back to the heap by marking
 * it as free and adding it to the free list. After marking the block as free,
 * the function attempts to coalesce (merge) it with adjacent free blocks to
 * reduce fragmentation.
 *
 * The freeing process includes the following steps:
 * 1. Convert the pointer to the memory block back to a pointer to the block's
 *    metadata by adjusting the pointer to point to the start of the block's metadata.
 * 2. Mark the block as free by setting the appropriate flag in the block's size field.
 * 3. Add the block to the head of the free list:
 *    - Set the block's `next` pointer to the current head of the free list.
 *    - Set the block's `prev` pointer to `NULL` as it will be the new head.
 *    - If the free list is not empty, update the previous head's `prev` pointer to
 *      point back to the new block.
 *    - Update the `free_list_head` to point to the new block.
 * 4. Attempt to coalesce the newly freed block with adjacent free blocks by calling
 *    `hmm_coalesce`. This helps to reduce fragmentation by merging contiguous free blocks.
 *
 * This function is used internally by the heap memory manager to free memory and is
 * not intended to be called directly by user code.
 *
 * @param ptr A pointer to the memory block to be freed. The pointer must have been
 *            previously allocated by `hmm_internal_alloc` or a similar function.
 */
void hmm_internal_free(void* ptr) 
{
    if (!ptr) return;
    
    block_metadata_t* block = (block_metadata_t*)ptr; // (block_metadata_t*)ptr - 1 is performed in HmmFree()
    
    /* Double check again after checking from HmmFree*/
    if (block->magic != MAGIC_NUMBER) 
    {
        last_error = HMM_ERROR_INVALID_POINTER;
        return;
    }
    
    if (block->size_and_flags & IS_FREE_MASK) {
        last_error = HMM_ERROR_DOUBLE_FREE;
        return;
    }
    

    /* You must insert first the block, so that we can call Merging function(hmm_coalesce) performed without bugs*/
    block->size_and_flags |= IS_FREE_MASK;
    
    block->next = free_list_head;
    block->prev = NULL;

    if (free_list_head) free_list_head->prev = block;

    free_list_head = block;

    /*Merge Free spaces if possible*/
    hmm_coalesce(block);
}







/**
 * Coalesces adjacent free memory blocks.
 *
 * This function merges a newly freed block with any adjacent free blocks in the
 * heap, reducing fragmentation by creating larger contiguous blocks of free memory.
 * The coalescing process involves checking both before and after the given block
 * for adjacent free blocks, and merging them if they are contiguous.
 *
 * The coalescing process includes the following steps:
 * 1. Iterate through the free list to find blocks that are adjacent to the given block:
 *    - If the current block is immediately before the freed block in memory, merge
 *      the two blocks by adding the sizes and updating the `size_and_flags` field of
 *      the current block.
 *    - If the freed block is immediately before the current block in memory, merge
 *      the two blocks by adding the sizes and updating the `size_and_flags` field of
 *      the freed block.
 * 2. In each case of merging, remove the smaller block from the free list using
 *    `hmm_remove_from_free_list` to avoid fragmentation within the free list itself.
 * 3. Continue checking the rest of the free list to ensure no other adjacent blocks
 *    are missed.
 *
 * This function is typically called after a block of memory is freed to ensure that
 * the heap remains as contiguous as possible, improving the efficiency of future
 * allocations.
 *
 * Note: The Merged  Node must be already in the free list so that hmm_coalesce works without bugs
 *
 * @param block A pointer to the block metadata of the newly freed block that is
 *              to be coalesced with adjacent free blocks.
 */
void hmm_coalesce(block_metadata_t* block) 
{
    // Start from the head of the free list
    block_metadata_t* curr = free_list_head;

    while (curr != NULL) 
    {
        block_metadata_t* next = curr->next;

        // Check if the current block is immediately before the freed block
        if ((char*)curr + (curr->size_and_flags & SIZE_MASK) + sizeof(block_metadata_t) == (char*)block) 
        {
            // Merge the current block with the freed block
            curr->size_and_flags = (curr->size_and_flags & SIZE_MASK) + 
                                   (block->size_and_flags & SIZE_MASK) + 
                                   sizeof(block_metadata_t);                                   
            curr->size_and_flags |= IS_FREE_MASK;
            
            // Remove the freed block from the free list
            hmm_remove_from_free_list(block); // Note: For this function the inserted block needed to be merged must be inserted in free list or bugs will happen.

            block = curr;  // Update block to the newly merged block
        } 

        // Check if the freed block is immediately before the current block
        else if ((char*)block + (block->size_and_flags & SIZE_MASK) + sizeof(block_metadata_t) == (char*)curr) 
        {
            // Merge the freed block with the current block
            block->size_and_flags = (block->size_and_flags & SIZE_MASK) + 
                                    (curr->size_and_flags & SIZE_MASK) + 
                                    sizeof(block_metadata_t);
            block->size_and_flags |= IS_FREE_MASK;
            
            // Remove the current block from the free list
            hmm_remove_from_free_list(curr);
        }

        curr = next;
    }
}








/**
 * Splits a block of memory into two smaller blocks.
 *
 * This function divides a larger memory block into two smaller blocks if the
 * block is sufficiently large to accommodate the requested size plus the minimum
 * allocation size and the metadata for a new block. The original block is resized
 * to the requested size, and a new block is created with the remaining memory.
 * The new block is added to the free list.
 *
 * The splitting process includes the following steps:
 * 1. Calculate the size of the original block by masking the `size_and_flags` field.
 * 2. Check if the original block has enough space to split:
 *    - Ensure that the remaining space after allocating the requested size is
 *      at least as large as the minimum allocation size plus the size of the
 *      block metadata.
 * 3. If splitting is possible:
 *    - Create a new block starting immediately after the requested size within
 *      the original block.
 *    - Set the size and flags for the new block and mark it as free.
 *    - Set the magic number for the new block to verify its integrity.
 *    - Add the new block to the head of the free list to make it available for
 *      future allocations.
 * 4. Update the original block's size to the requested size.
 *
 * This function is used internally by the heap memory manager to optimize the
 * utilization of memory by reducing fragmentation and making smaller blocks
 * available for future allocations.
 *
 * Note1: hmm_split_block desn't Remove the remainning allocated block from the list, so you must remove it if you want to allocate it.
 * Note2: The entered pointer must points to the beginning of metadata.
 *
 * @param block A pointer to the block metadata of the block to be split.
 * @param size The size of the memory to allocate from the block.
 */
void hmm_split_block(block_metadata_t* block, size_t size) 
{
    // Calculate the size of the original block
    size_t block_size = block->size_and_flags & SIZE_MASK;
    
    // Check if the block can be split
    if (block_size - size >= MIN_ALLOC_SIZE + sizeof(block_metadata_t)) 
    {
        // Create a new block with the remaining memory
        block_metadata_t* new_block = (block_metadata_t*)((char*)block + sizeof(block_metadata_t) + size);
        new_block->size_and_flags = ((block_size - size - sizeof(block_metadata_t)) & SIZE_MASK) | IS_FREE_MASK;
        new_block->magic = MAGIC_NUMBER;

        // Add the new block to the head of the free list
        new_block->next = free_list_head;
        new_block->prev = NULL;
        if (free_list_head)
            free_list_head->prev = new_block;
        free_list_head = new_block;

        // Update the original block's size to the requested size, and set status bit as allocated not free(set it to 0).
        block->size_and_flags = size & SIZE_MASK;

        // Remove allocated block from free list
        hmm_remove_from_free_list(block);

    }
}








/**
 * Finds a suitable free block of memory.
 *
 * This function searches the free list to find a block of memory that can satisfy
 * the requested size. The search algorithm depends on the current allocation strategy,
 * which can be `FIRST_FIT`, `BEST_FIT`, or `WORST_FIT`. The function returns a pointer
 * to the best-matching block or `NULL` if no suitable block is found.
 *
 * The block search process includes the following steps:
 * 1. Initialize the best-matching block to `NULL`.
 * 2. Iterate through the free list:
 *    - For each block, verify its integrity by checking the magic number.
 *    - Ensure that the block is free by checking the free flag.
 *    - Compare the block size to the requested size:
 *      - If the current algorithm is `FIRST_FIT`, return the first block that is large
 *        enough to satisfy the request.
 *      - If the current algorithm is `BEST_FIT`, update the best block if the current
 *        block is smaller than the previously found best block and still large enough.
 *      - If the current algorithm is `WORST_FIT`, update the best block if the current
 *        block is larger than the previously found best block.
 * 3. Return the best-matching block or `NULL` if no suitable block is found.
 *
 * This function is used internally by the heap memory manager to locate free blocks
 * for memory allocation requests.
 *
 * @param size The size of the memory to allocate.
 * @return A pointer to the best-matching block if found; otherwise, `NULL`.
 */
block_metadata_t* hmm_find_free_block(size_t size) 
{
    // Initialize the best-matching block to NULL
    block_metadata_t* best = NULL;

    // Iterate through the free list
    for (block_metadata_t* block = free_list_head; block != NULL; block = block->next) 
    {
        // Verify the block's integrity and free status
        assert(block->magic == MAGIC_NUMBER);
        assert(block->size_and_flags & IS_FREE_MASK);

        // Get the size of the current block
        size_t block_size = block->size_and_flags & SIZE_MASK;

        // Check if the block can satisfy the requested size
        if (block_size >= size) 
        {
            // Update the best-matching block based on the allocation strategy
            switch (current_algorithm) 
            {
                case FIRST_FIT:
                    return block;

                case BEST_FIT: // Future Work
                    if (!best || block_size < (best->size_and_flags & SIZE_MASK)) 
                    {
                        best = block;
                    }
                    break;
                    
                case WORST_FIT: // Future Work
                    if (!best || block_size > (best->size_and_flags & SIZE_MASK)) 
                    {
                        best = block;
                    }
                    break;
            }
        }
    }

    // Return the best-matching block or NULL if none was found
    return best;
}








/**
 * Allocates memory from the heap.
 *
 * This function attempts to allocate a block of memory from the heap using
 * the internal allocation function. If the allocation is successful, it
 * returns a pointer to the allocated memory. If the allocation fails, it
 * sets an error code to indicate out-of-memory conditions.
 *
 * The allocation process includes the following steps:
 * 1. Call the internal allocation function `hmm_internal_alloc` to request
 *    the specified amount of memory.
 * 2. Check if the allocation was successful:
 *    - If successful, return the pointer to the allocated memory.
 *    - If unsuccessful (i.e., the allocation returned NULL), set the global
 *      error variable `last_error` to `HMM_ERROR_OUT_OF_MEMORY` to indicate
 *      that the heap is out of memory.
 *
 * @function HmmAlloc
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory if successful; otherwise, NULL.
 */
void* HmmAlloc(size_t size) 
{
    // Attempt to allocate the requested memory size
    void* result = hmm_internal_alloc(size);
    
    // Check if allocation failed
    if (!result) 
    {
        // Set the global error code to indicate out-of-memory error
        last_error = HMM_ERROR_OUT_OF_MEMORY;
    }
    
    // Return the result of the allocation attempt
    return result;
}






/**
 * Frees a previously allocated memory block.
 *
 * This function releases a memory block that was previously allocated by
 * `HmmAlloc`. It performs several checks to ensure the validity of the pointer,
 * prevents double freeing, and then calls the internal free function to handle
 * the actual deallocation.
 *
 * @param ptr Pointer to the memory block to be freed. It must be a valid pointer
 *            returned by `HmmAlloc`. If NULL, the function does nothing.
 */
void HmmFree(void* ptr) 
{
    // If the pointer is NULL, do nothing
    if (!ptr) return;
    
    // Retrieve the block_metadata_t structure for the block to be freed
    block_metadata_t* block = (block_metadata_t*)ptr - 1;
    
    // Check if the block's magic number is correct
    // This ensures that the block has not been corrupted or invalid
    if (block->magic != MAGIC_NUMBER) 
    {
        last_error = HMM_ERROR_INVALID_POINTER;
        //printf("Invalid magic number or you enter invalid pointer\n");
        return;
    }
    
    // Check if the block is already marked as free
    // If so, it indicates a double free attempt
    if (block->size_and_flags & IS_FREE_MASK) 
    {
        last_error = HMM_ERROR_DOUBLE_FREE;
        //printf("Double free detected\n");
        return;
    }
    
    // Call the internal free function to handle the actual deallocation
    hmm_internal_free(ptr);
}







/**
 * Retrieves the last error code set by the heap manager.
 *
 * This function returns the most recent error code that was recorded by the
 * heap manager during the allocation or deallocation of memory.
 *
 * @return The last error code of type hmm_error_t.
 */
hmm_error_t hmm_get_last_error(void) 
{
    return last_error;
}





/**
 * Sets the memory allocation algorithm used by the heap manager.
 *
 * This function allows the caller to specify which memory allocation algorithm
 * should be used by the heap manager. The algorithm determines how free blocks
 * of memory are selected for allocation. The available algorithms are:
 * FIRST_FIT, BEST_FIT, and WORST_FIT.
 *
 * @param algorithm The allocation algorithm to be used, of type hmm_alloc_algorithm_t.
 *                  The caller can choose from FIRST_FIT, BEST_FIT, or WORST_FIT.
 */
void hmm_set_allocation_algorithm(hmm_alloc_algorithm_t algorithm) 
{
    current_algorithm = algorithm;
}




/*============================================================================
 *************************  Standard HMM Functions  **************************
 ============================================================================*/ 
/**
 * Allocates a block of memory of the specified size.
 *
 * This function first ensures that the heap is initialized by calling `hmm_init`
 * if the heap has not been set up yet. It then calls `HmmAlloc` to allocate
 * memory of the requested size. If successful, it returns a pointer to the allocated
 * memory block. If the heap is not initialized or allocation fails, the function
 * handles these cases appropriately.
 *
 * @param size The number of bytes to allocate.
 *
 * @return A pointer to the allocated memory block if successful, or NULL if
 *         the allocation fails or the heap is not initialized.
 */
void* malloc(size_t size) 
{
    // Ensure the heap is initialized
    if (!heap_start) hmm_init();

    // Allocate the requested memory using HmmAlloc
    return HmmAlloc(size);
}







/**
 * Frees a previously allocated block of memory.
 *
 * This function deallocates the memory block pointed to by `ptr` by calling
 * the `HmmFree` function. It is used to release memory that was previously
 * allocated by `malloc`, `calloc`, or `realloc`.
 *
 * @param ptr A pointer to the memory block to free. This pointer must have been
 *            previously allocated by one of the memory allocation functions.
 */
void free(void* ptr) 
{
    // Free the memory block using HmmFree
    HmmFree(ptr);
}







/**
 * Allocates and zero-initializes a block of memory.
 *
 * This function calculates the total memory required to allocate `nmemb` elements
 * each of size `size`. It checks for overflow during the multiplication to avoid
 * allocation errors. If overflow occurs, it sets an error flag and returns NULL.
 * Otherwise, it allocates the required memory using `malloc` and then initializes
 * the allocated memory to zero using `memset`. The pointer to the zero-initialized
 * memory block is returned.
 *
 * @param nmemb The number of elements to allocate.
 * @param size The size of each element.
 *
 * @return A pointer to the zero-initialized memory block if successful, or NULL
 *         if the memory allocation fails or overflow occurs.
 */
void* calloc(size_t nmemb, size_t size) 
{
    size_t total_size;

    // Check for overflow during multiplication
    if (__builtin_mul_overflow(nmemb, size, &total_size)) 
    {
        // Set error flag and return NULL if overflow occurs
        last_error = HMM_ERROR_OUT_OF_MEMORY;
        return NULL;
    }
    
    // Allocate the required memory
    void* ptr = malloc(total_size);

    // Zero-initialize the allocated memory if successful
    if (ptr) 
    {
        memset(ptr, 0, total_size);
    }

    return ptr;
}






/**
 * Resizes a previously allocated block of memory.
 *
 * This function adjusts the size of the memory block pointed to by `ptr` to the
 * new size specified by `size`. If `ptr` is NULL, it behaves like `malloc` and
 * allocates a new block of the specified size. If `size` is zero, it behaves like
 * `free` and deallocates the memory block. If `size` is larger than the current
 * size of the block, a new block is allocated, the contents of the old block are
 * copied to the new block, and the old block is freed.
 *
 * @param ptr A pointer to the memory block to resize. This pointer must have been
 *            previously allocated by one of the memory allocation functions.
 * @param size The new size of the memory block.
 *
 * @return A pointer to the resized memory block if successful, or NULL if the
 *         allocation fails or the old block could not be resized.
 */
void* realloc(void* ptr, size_t size) 
{
    // If the pointer is NULL, behave like malloc
    if (!ptr) return malloc(size);

    // If the size is zero, behave like free
    if (size == 0) 
    {
        free(ptr);
        return NULL;
    }
    
    // Retrieve the metadata for the existing block
    block_metadata_t* block = (block_metadata_t*)ptr - 1;
    size_t old_size = block->size_and_flags & SIZE_MASK;
    
    // If the new size is smaller or equal, split the block if necessary
    if (size <= old_size) 
    {
        hmm_split_block(block, size);
        return ptr;
    }
    
    // Allocate a new block of the requested size
    void* new_ptr = malloc(size);
    if (!new_ptr) return NULL;
    
    // Copy the contents of the old block to the new block
    memcpy(new_ptr, ptr, old_size);

    // Free the old block
    free(ptr);
    
    return new_ptr;
}






/*============================================================================
 ***************************  Old Code Graveyard  ****************************
 ============================================================================*/ 
/**
 * Initializes the heap memory manager.
 *
 * This function sets up the initial state of the heap memory manager by
 * initializing the free list and setting the program break pointer. It
 * prepares the system for subsequent memory allocation and deallocation
 * operations.
 *
 * The initialization process includes the following steps:
 * 1. Set the `free_list_head` pointer to `NULL` to indicate that the free list
 *    is initially empty.
 * 2. Set the `program_break` pointer to the beginning of the heap, establishing
 *    the starting point for memory allocations.
 *
 * This function should be called once at the start of the program to ensure that
 * the heap memory manager is properly initialized before any memory operations
 * are performed.
 *
 */
/*void hmm_init(void) 
{
    // Initialize with an empty free list
    free_list_head = NULL;

    // Set the program break to the start of the heap
    program_break = heap;
}*/




/**
 * Cleans up and resets the heap manager's internal state.
 *
 * This function resets the heap and free list to their initial states. It clears
 * all data in the heap to ensure that no runtime data remains, sets the free list
 * head to NULL, and resets the program break to the start of the heap. This is
 * useful for resetting the heap manager to a clean state, typically before shutting
 * down or reinitializing the system.
 *
 * @note This operation clears all allocated memory, so it should be used with caution.
 */
/*void hmm_cleanup(void) 
{
    // Reset the heap and free list
    // clear all heap data(for secure purpose)
    memset(heap, 0, HEAP_SIZE); 

    // Reset Head of free list
    free_list_head = NULL;

    // Reset Program_break
    program_break = heap;
}
*/







/**
 * Helper function to retrieve the next physically contiguous memory block.
 *
 * This function calculates the address of the next block in memory by
 * adding the size of the current block to its starting address. It then
 * checks if this address falls within the valid heap range and verifies
 * that the block at this address has a valid magic number. If these conditions
 * are met, the function returns a pointer to the next block; otherwise, it returns NULL.
 *
 * @param block Pointer to the block_metadata_t structure of the current block.
 *              This block is used to determine the starting address of the next block.
 * 
 * @return Pointer to the next block if it is valid and within heap range; otherwise, NULL.
 */
/*static block_metadata_t* get_next_physical_block(block_metadata_t* block) 
{
    // Calculate the address of the next block by adding the size of the current block
    char* next = (char*)block + (block->size_and_flags & SIZE_MASK);
    
    // Check if the calculated address is within the heap's valid range
    if (next >= heap && next < heap + HEAP_SIZE) 
    {
        // Cast the address to a block_metadata_t pointer
        block_metadata_t* next_block = (block_metadata_t*)next;

        // Verify that the next block has a valid magic number
        if (next_block->magic == MAGIC_NUMBER) 
        {
            return next_block;
        }
    }
    // Return NULL if the address is invalid or the block is not valid
    return NULL;
}
*/



// Static array modeling the heap memory managed by the custom heap manager.
// Its size is defined by the HEAP_SIZE macro, which is typically set to a value like 1 MB.
//static char heap[HEAP_SIZE];




// Defines the total size of the heap memory managed by the heap manager.
// 1048576 bytes (1 MB) is a typical size used for testing or small-scale applications.
//#define HEAP_SIZE          1000000000


/** This ensures that no single allocation exceeds a manageable size, 
 *  which could prevent over-committing memory or could be used for optimizing memory management algorithms.
 *
 * It may also serve as a safeguard to ensure that any requests for memory allocation exceeding SIZE_MAX
 * are either handled differently or flagged as an error.
 */
//#define SIZE_MAX         10000
