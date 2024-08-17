# Sustem Design Requirements

## Model of Heap:
1- We model Heap as char MyHeap[HEAP_LMIT_SIZE]; which is a very big global array of chars that is declared before main.
2- We model Program Break pointer as ProgramBreak_Ptr which is initialized as char *ProgramBreak_Ptr = MyHeap;

## Linked List implementation to track free memory blocks in MyHeap:
1- Structure:
* Metadata(takes size of unsigned int)  that contains the size of free memory block Node, and the-
  last bit of Metadata is 1 if it is non-free block, and 0 if it is free block.
* Previous int pointer PreviousFree_Ptr that points to the previous Free memory block, it should be NULL if it is first node.
* Next int pointer NextFree_Ptr that points to the next Free memory block, it should be NULL if it is last node.

2- Head:
* Head pointer is a global pointer, that points to the first free memory block(That contains the least memory address).
* The Head should point to the beginning of Metadata of the first free memory block.
* If lower address free block is entered, The head should points to it.

3- Algorithm: 
* The free memory blocks nodes must be ordered in ascending order,
  Example: Three memory blocks exists in the linked list, From address (MyHeap+100) to (MyHeap+200), 
  From address (MyHeap+400) to (MyHeap+700), From address (MyHeap+1000) to (MyHeap+1200).
  if we want to insert a free memory block that its address range is from address (MyHeap+800) to (MyHeap+900),
  The the new linked List should be like that:
  (MyHeap+100) to (MyHeap+200), (MyHeap+400) to (MyHeap+700), (MyHeap+800) to (MyHeap+900), (MyHeap+1000) to (MyHeap+1200).
* For any new free memory block, The size of this free block must be stored in MetaData of this node and the last bit in MetaData must set to 0.
* For any new inserted free block the linked list must make sure if there is any free blocks that are contigous to merge them in one free block.

4- FreeMemoryBlocks_LinkedList elements actual storage :
* All informations like previous free pointer, Next free pointer and Metadata, are stored in MyHeap memory-
  in a contigous structure block that takes the content of the linked List structure and store it in MyHeap.


## Algorithm of Allocation:
1- Define a configred global variable that contains algorithm type of allocation-
   i.e. AllocationAlgorithm should contain FIRST_FIT or BEST_FIT or WORST_FIT.
2- If user wants to allocate memory and there is in the FreeMemoryBlocks_LinkedList more than one suitable free block,
   Based on configration in AllocationAlgorithm, We should choose which free block should be allocated.

## Steps of Allocation:
1- First HmmAlloc call:
* Check if this is the firt memory alloc, by checking if the ProgramBreak_Ptr ==  MyHeap && Head == NULL.
* Move the the ProgramBreak_Ptr with a factor from the desired size of allocation; For Example:
  if  user needs to allocate 500 byte, then move program break like that, ProgramBreak_Ptr = ProgramBreak_Ptr + 500*10;
* store in Metadata the size of desired allocated data and set the last bit in it to 1, To indicate that this block-
  is allocated block.
* Insert free memory block created(that is between the size of Metadata + size of Allocated memory and the ProgramBreak_Ptr)-
  to the FreeMemoryBlocks_LinkedList.
* return to user a pointer that points to the allocated block of memory(That points exactly after Metadata location).

2- Ordinary HmmAlloc call:
* Check the FreeMemoryBlocks_LinkedList if there is any block is free enough to allocate the desired memory bytes, if not-
  move ProgramBreak_Ptr to increase size of heap and do the same steps of first HmmAlloc call.
* If there is suitable free blocks, allocate any one of them based on configred algorithm in AllocationAlgorithm.
* If the free block allocated is bigger than desired memory allocation, split it to the allocated part,
  and the remainning free block from that split insert it in FreeMemoryBlocks_LinkedList.
* Return to user a pointer that points to the allocated block of memory(That points exactly after Metadata location).


## Steps of Free:
1- HmmFree call: 
* we should see the metadata location by decremnting the pointer, then cast this address to structure-
  of FreeMemoryBlocks_LinkedList(Metadata, PreviousFree_Ptr, NextFree_Ptr).
* PreviousFree_Ptr and NextFree_Ptr should be initialized with NULL.
* Insert this free node to FreeMemoryBlocks_LinkedList.


