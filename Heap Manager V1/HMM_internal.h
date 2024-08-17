/**
 *===================================================================================
 * @file           : HMM_internal.h
 * @author         : Ali Mamdouh
 * @brief          : Header internal interface of Heap manager, The code is split into public (HMM.h) and internal (HMM_internal.h) headers, 
 *                   separating the interface from the implementation details.
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
 *===================================================================================
 * 
 *===================================================================================
 */



// hmm_internal.h
#ifndef HMM_INTERNAL_H
#define HMM_INTERNAL_H

/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include "HMM.h"





/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/
/**
 * Magic number used for validating block metadata.
 *
 * This constant defines a specific value that is used as a "magic number" in
 * the metadata of memory blocks. It helps to verify the integrity of the memory
 * block's metadata by ensuring that the block's data has not been CORRUPTED.
 *
 */
#define MAGIC_NUMBER             0xDEADBEEF


/**
 * Bitmask to check if a memory block is free.
 *
 * This constant defines a bitmask used to determine whether a memory block
 * is currently free or allocated. The mask isolates the bit that indicates
 * the block's status.
 *
 */
#define IS_FREE_MASK             0x1


/**
 * Bitmask to extract the size of a memory block from its metadata.
 *
 * This constant defines a bitmask used to extract the size of a memory block
 * from the block's metadata. It clears the bit used for indicating whether the
 * block is free or allocated, leaving only the size information.
 *
 * @constant SIZE_MASK
 * @value (~IS_FREE_MASK)
 */
#define SIZE_MASK                (~IS_FREE_MASK)


#define FACTOR_OF_ALLOCATION     0x09





/*============================================================================
 *************************  Data types Declerations  *************************
 ============================================================================*/
// MetaData block 
typedef struct block_metadata 
{
    size_t size_and_flags;       // Size of the block and status flags
    struct block_metadata* prev; // Pointer to the previous block in the free list
    struct block_metadata* next; // Pointer to the next block in the free list
    unsigned int magic;          // Magic number for integrity check
} block_metadata_t;





/*============================================================================
 **************************  Functions  Decleration  *************************
 ============================================================================*/
void* hmm_internal_alloc(size_t size);
void hmm_internal_free(void* ptr);
void hmm_coalesce(block_metadata_t* block);
block_metadata_t* hmm_find_free_block(size_t size);
void hmm_split_block(block_metadata_t* block, size_t size);



#endif // HMM_INTERNAL_H
