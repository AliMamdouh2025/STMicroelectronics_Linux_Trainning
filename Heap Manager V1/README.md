# Project Build Test Example

To build and execute the Test, follow these steps:

1. Compile and run the source example test files with the following command:
   ```
   gcc -g HMM.c stress_test.c -o hmm_example && ./hmm_example 
   ```






# Flow of API usage

![image](https://github.com/user-attachments/assets/1e4b7944-3920-42d9-95fb-1806e94cadd9)






# Flow chart of HmmFree API

![image](https://github.com/user-attachments/assets/c70137c3-448a-4406-8b8e-18bc4ec434e8)





# Flow chart of HmmAlloc API

![image](https://github.com/user-attachments/assets/d5cf5408-c583-40dd-bbc8-9b81ac81c01f)






# System Design Requirements

## Model of Heap:
1- We model Heap as char heap[HEAP_LMIT_SIZE]; which is a very big global array of chars that is declared before main.
2- We model Program Break pointer as ProgramBreak_Ptr which is initialized as char 
   *ProgramBreak_Ptr = heap;

## Circular Linked List implementation to track free memory blocks in heap:
1- Structure:
* Metadata(takes size of unsigned int)  that contains the size of free memory block Node, and the-
  least bit of Metadata is 0 if it is non-free block, and 1 if it is free block.
* Previous int pointer PreviousFree_Ptr that points to the previous Free memory block.
* Next int pointer NextFree_Ptr that points to the next Free memory block.

2- Head:
* Head pointer is a global pointer, that points to the first free memory block(That contains the 
  least memory address).
* The Head should point to the beginning of Metadata of the first free memory block.
* If lower address free block is entered, The head should points to it.

3- Algorithm: 
* The free memory blocks nodes must be ordered in ascending order,
  Example: Three memory blocks exists in the linked list, From address (heap+100) to (heap+200), 
  From address (heap+400) to (heap+700), From address (heap+1000) to (heap+1200).
  if we want to insert a free memory block that its address range is from address (heap+800) to 
  (heap+900), The the new linked List should be like that:
  (heap+100) to (heap+200), (heap+400) to (heap+700), (heap+800) to (heap+900), (heap+1000) to 
  (heap+1200).
* For any new free memory block, The size of this free block must be stored in MetaData of this node and the least bit in MetaData must set to 1.
* For any new inserted free block the linked list must make sure if there is any free blocks that 
  are contigous to merge them in one free block.

4- FreeMemoryBlocks_LinkedList elements actual storage :
* All informations like previous free pointer, Next free pointer and Metadata, are stored in heap memory-
  in a contigous structure block that takes the content of the linked List structure and store it 
  in heap.


## Algorithm of Allocation:
1- Define a configred global variable that contains algorithm type of allocation-
   i.e. AllocationAlgorithm should contain FIRST_FIT or BEST_FIT or WORST_FIT.
2- If user wants to allocate memory and there is in the FreeMemoryBlocks_LinkedList more than one 
   suitable free block, Based on configration in AllocationAlgorithm, We should choose which free 
   block should be allocated.

## Steps of Allocation:
1- First HmmAlloc call:
* Check if this is the firt memory alloc, by checking if the ProgramBreak_Ptr ==  heap && Head == 
  NULL.
* Move the the ProgramBreak_Ptr with a factor from the desired size of allocation; For Example:
  if  user needs to allocate 500 byte, then move program break like that, ProgramBreak_Ptr = 
  ProgramBreak_Ptr + 500*10;
* store in Metadata the size of desired allocated data and set the least bit in it to 1, To indicate that this block-
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
* Insert this free node to FreeMemoryBlocks_LinkedList.


