/**
 *===================================================================================
 * @file           : HMM.c
 * @author         : Ali Mamdouh
 * @brief          : Source of heap manager
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
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





/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/





/*============================================================================
 **************************  Variables Definitions  **************************
 ============================================================================*/
// Static array modeling the heap memory managed by the custom heap manager.
// Its size is defined by the HEAP_SIZE macro, which is typically set to a value like 1 MB.
static char heap[HEAP_SIZE];

// Pointer to the head of the free list, which keeps track of free memory blocks in the heap.
// Initially set to NULL, meaning no free blocks are available until the heap is initialized.
static block_metadata_t* free_list_head = NULL;

// Pointer to the current program break, representing the end of the allocated space in the heap.
// Initially starts at the beginning of the heap array.
static char* program_break = heap;

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
 * Setup the MODELED heap memory.
 *
 * This function sets up the initial state of the heap memory manager by configuring
 * the first memory block in the heap. It sets up the free list, initializes the
 * metadata of the first block, and sets the magic number for integrity checking.
 *
 * The initialization includes the following steps:
 * 1. Set the `free_list_head` pointer to point to the start of the heap.
 * 2. Configure the metadata of the initial memory block:
 *    - Set its size to the total heap size and mark it as free.
 *    - Initialize its `prev` and `next` pointers to point to itself, indicating a circular linked list.
 *    - Set the `magic` number to ensure data integrity.
 *
 */
void hmm_init(void) 
{
    // Set the start of the heap as the head of the initial virtual free list
    free_list_head = (block_metadata_t*)heap;
    
    // Initialize the metadata for the first block in the heap
    free_list_head->size_and_flags = HEAP_SIZE | IS_FREE_MASK; // Set size to total heap size and mark block as free
    free_list_head->prev = free_list_head->next = free_list_head; // Initialize the free list as a circular list with one block
    free_list_head->magic = MAGIC_NUMBER; // Set the magic number for integrity checking
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
        printf("Invalid magic number\n");
        return;
    }
    
    // Check if the block is already marked as free
    // If so, it indicates a double free attempt
    if (block->size_and_flags & IS_FREE_MASK) 
    {
        last_error = HMM_ERROR_DOUBLE_FREE;
        printf("Double free detected\n");
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
void hmm_cleanup(void) 
{
    // Reset the heap and free list
    // clear all heap data(for secure purpose)
    memset(heap, 0, HEAP_SIZE); 

    // Reset Head of free list
    free_list_head = NULL;

    // Reset Program_break
    program_break = heap;
}





/**
 * Internal memory allocation function.
 *
 * This function manages the allocation of memory from the heap by performing
 * the following steps:
 * 1. Align the requested memory size to the nearest multiple of the alignment
 *    requirement to ensure efficient memory access and management.
 * 2. Ensure that the size is not smaller than the minimum allocation size.
 * 3. Attempt to find a free block of memory that fits the requested size.
 *    - If a suitable free block is found, split the block if necessary and
 *      update its metadata to mark it as allocated.
 *    - If no suitable free block is available:
 *      - Extend the heap if there is enough space.
 *      - Allocate memory at the end of the heap and update the program break.
 * 4. Mark the allocated block as not free and return a pointer to the memory
 *    location right after the block's metadata.
 *
 * @function hmm_internal_alloc
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or NULL if allocation fails.
 */
void* hmm_internal_alloc(size_t size) 
{
    // Align the requested size to the nearest multiple of the alignment requirement
    size = ((size + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;

    // Ensure the size is at least the minimum allocation size
    if (size < MIN_ALLOC_SIZE) size = MIN_ALLOC_SIZE;
    
    // Attempt to find a free block that fits the requested size
    block_metadata_t* block = hmm_find_free_block(size);

    // If no suitable block is found, extend the heap if possible
    if (!block) 
    {
        if (program_break + size + sizeof(block_metadata_t) > heap + HEAP_SIZE) 
        {
            return NULL;  // Out of memory, unable to extend the heap
        }

        // Allocate a new block at the end of the heap
        block = (block_metadata_t*)program_break;
        block->size_and_flags = size;
        block->magic = MAGIC_NUMBER;
        program_break += size*FACTOR_OF_ALLOCATION + sizeof(block_metadata_t);
    } 
    else 
    {
        // If a free block is found, split it if necessary
        hmm_split_block(block, size);
    }
    
    // Mark the block as allocated
    block->size_and_flags &= ~IS_FREE_MASK;

    // Return a pointer to the memory right after the metadata
    return (void*)(block + 1);
}







/**
 * Internal function to handle the deallocation of a memory block.
 *
 * This function marks a memory block as free and inserts it into the free list.
 * It also performs coalescing to merge adjacent free blocks into a larger one,
 * reducing fragmentation and improving memory utilization.
 *
 * @param ptr Pointer to the memory block to be freed. This should be a valid
 *            pointer returned by `HmmAlloc`.
 */
void hmm_internal_free(void* ptr) 
{
    // Retrieve the block_metadata_t structure for the block to be freed
    block_metadata_t* block = (block_metadata_t*)ptr - 1;
    
    // Mark the block as free by setting the IS_FREE_MASK flag
    block->size_and_flags |= IS_FREE_MASK;
    
    // Insert the block into the free list
    if (free_list_head && free_list_head->next) 
    {
        // Insert the block at the beginning of the free list
        block->next = free_list_head->next;
        block->prev = free_list_head;
        free_list_head->next->prev = block;
        free_list_head->next = block;
    } else 
    {
        // Initialize the free list with the new block
        block->next = block;
        block->prev = block;
        free_list_head = block;
    }
    
    // Coalesce adjacent free blocks if exists to merge them into a larger block
    hmm_coalesce(block);
}






/**
 * Helper function to get the footer of a memory block.
 *
 * This function calculates the address of the footer for a given memory block
 * based on its size and the size of the metadata. The footer is located at the
 * end of the block, right before the next block’s metadata or the end of the 
 * allocated memory region.
 *
 * @param block Pointer to the block_metadata_t structure representing the block.
 * @return Pointer to the block_metadata_t structure at the end of the block, which is the footer.
 */
static block_metadata_t* get_footer(block_metadata_t* block) 
{
    return (block_metadata_t*)((char*)block + (block->size_and_flags & SIZE_MASK) - sizeof(block_metadata_t));
}






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
static block_metadata_t* get_next_physical_block(block_metadata_t* block) 
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






/**
 * Coalesce adjacent free memory blocks to create larger contiguous free blocks.
 *
 * This function merges the current block with its adjacent free blocks (both
 * previous and next blocks) if they are free. This helps reduce fragmentation
 * in the heap by combining smaller free blocks into larger ones. The function
 * also updates the free list and metadata to maintain consistency.
 *
 * @param block Pointer to the block_metadata_t structure of the block to be coalesced.
 *              This block is assumed to be free.
 */
void hmm_coalesce(block_metadata_t* block) 
{
    // Ensure the block's metadata is valid
    assert(block->magic == MAGIC_NUMBER);
    assert(block->size_and_flags & IS_FREE_MASK);

    // Get the next physically contiguous block
    block_metadata_t* next_phys = get_next_physical_block(block);
    block_metadata_t* prev_phys = NULL;

    // Check if there is a previous block and if it's valid
    if (block > (block_metadata_t*)heap) 
    {
        // Access the footer of the previous block
        block_metadata_t* prev_footer = (block_metadata_t*)((char*)block - sizeof(block_metadata_t));

        if (prev_footer->magic == MAGIC_NUMBER) 
        {
            // Retrieve the previous block's metadata
            prev_phys = (block_metadata_t*)((char*)block - (prev_footer->size_and_flags & SIZE_MASK));

            if (prev_phys < (block_metadata_t*)heap || prev_phys->magic != MAGIC_NUMBER) 
            {
                prev_phys = NULL;
            }
        }
    }

    // Coalesce with the next block if it is free
    if (next_phys && (next_phys->size_and_flags & IS_FREE_MASK)) 
    {
        // Extend the current block to include the next block
        block->size_and_flags += next_phys->size_and_flags & SIZE_MASK;

        // Remove the next block from the free list
        if (next_phys->prev && next_phys->next) 
        {
            next_phys->prev->next = next_phys->next;
            next_phys->next->prev = next_phys->prev;
        }
    }

    // Coalesce with the previous block if it is free
    if (prev_phys && (prev_phys->size_and_flags & IS_FREE_MASK)) 
    {
        // Extend the previous block to include the current block
        prev_phys->size_and_flags += block->size_and_flags & SIZE_MASK;

        // Remove the current block from the free list
        if (block->prev && block->next) 
        {
            block->prev->next = block->next;
            block->next->prev = block->prev;
        }

        // Update the pointer to the new coalesced block
        block = prev_phys;
    }

    // Update the footer of the coalesced block
    block_metadata_t* footer = get_footer(block);
    if (footer >= (block_metadata_t*)heap && footer < (block_metadata_t*)(heap + HEAP_SIZE)) 
    {
        footer->size_and_flags = block->size_and_flags;
        footer->magic = MAGIC_NUMBER;
    }

    // Debug output for verifying coalescing operation
    printf("Coalescing completed. Final block size: %zu\n", block->size_and_flags & SIZE_MASK); // For Debugging
}






/**
 * Finds a suitable free block from the list of free memory blocks.
 *
 * This function searches through the linked list of free memory blocks to
 * find one that meets the size requirement specified. The search strategy
 * depends on the current allocation algorithm, which could be:
 * - **FIRST_FIT**: Returns the first block that is large enough.
 * - **BEST_FIT**: Placeholder for future implementation to return the smallest block
 *   that is large enough.
 * - **WORST_FIT**: Placeholder for future implementation to return the largest block
 *   that is large enough.
 *
 * The function also performs integrity checks to ensure that each block's
 * metadata is valid and that the block is marked as free.
 *
 * @function hmm_find_free_block
 * @param size The size of the memory block to allocate.
 * @return A pointer to a free block that is large enough to satisfy the
 *         request, or NULL if no suitable block is found.
 */
block_metadata_t* hmm_find_free_block(size_t size) 
{
    // Pointer to the best fitting block found
    block_metadata_t* best = NULL; 

    // size_t best_size = SIZE_MAX; This variable will be used for future work
    // of using the BEST_FIT or WORST_FIT allocation algorithms

    // Iterate through the free list starting from the head
    for (block_metadata_t* block = free_list_head->next; block != free_list_head; block = block->next) 
    {
        // Ensure the block's metadata is valid
        assert(block->magic == MAGIC_NUMBER);
        assert(block->size_and_flags & IS_FREE_MASK);

        // Extract the size of the current block
        size_t block_size = block->size_and_flags & SIZE_MASK; //////////////////////////////////////////////

        // Check if the current block is large enough
        if (block_size >= size) 
        {
            // Apply the allocation strategy to choose the best block
            switch (current_algorithm) 
            {
                case FIRST_FIT:
                    // Return the first block that is large enough
                    return block;

                case BEST_FIT:
                    // Placeholder for future work: Implement logic to select the smallest
                    // block that is large enough
                    break;

                case WORST_FIT:
                    // Placeholder for future work: Implement logic to select the largest
                    // block that is large enough
                    break;
            }
        }
    }

    // Return the best fitting block, or NULL if none was found
    return best;
}






/**
 * Splits a memory block into two separate blocks if there is enough space.
 *
 * This function splits a larger free memory block into two smaller blocks. 
 * One block will be allocated to fulfill the request, while the other block 
 * will remain in the free list. The function updates the metadata and free list 
 * to reflect the new blocks, ensuring proper memory management and reducing 
 * fragmentation.
 *
 * @function hmm_split_block
 * @param block Pointer to the memory block that is to be split.
 * @param size The size of the block required for allocation.
 */
void hmm_split_block(block_metadata_t* block, size_t size) 
{
    // Verify the integrity of the block's metadata
    assert(block->magic == MAGIC_NUMBER);

    // Check that the block is marked as free
    assert(block->size_and_flags & IS_FREE_MASK);

    // Extract the current block's size from its metadata
    size_t block_size = block->size_and_flags & SIZE_MASK;
    
    // Ensure that there is enough space left after splitting for a new block and its metadata
    if (block_size - size >= MIN_ALLOC_SIZE + sizeof(block_metadata_t)) 
    {
        // Calculate the address of the new block by moving past the current block's allocated size and its metadata
        block_metadata_t* new_block = (block_metadata_t*)((char*)block + size + sizeof(block_metadata_t));
        
        // Initialize the new block's metadata
        new_block->size_and_flags = (block_size - size - sizeof(block_metadata_t)) | IS_FREE_MASK; // Mark new block as free
        new_block->magic = MAGIC_NUMBER; // Set magic number for integrity checking

        // Update the free list to include the new block
        new_block->prev = block->prev;
        new_block->next = block->next;
        block->prev->next = new_block;
        block->next->prev = new_block;

        // Update the original block's metadata to reflect the allocated size
        block->size_and_flags = size;

        // Update the footers of both blocks to reflect their sizes and magic numbers
        block_metadata_t* new_footer = get_footer(new_block);
        new_footer->size_and_flags = new_block->size_and_flags;
        new_footer->magic = MAGIC_NUMBER;

        block_metadata_t* footer = get_footer(block);
        footer->size_and_flags  = block->size_and_flags;
        footer->magic = MAGIC_NUMBER;
    }
}
