// main.c : Exmple usage of heap manager
#include <stdio.h>
#include "HMM.h"


int main() 
{
    hmm_init();

    //hmm_set_allocation_algorithm(BEST_FIT); Future work

    int* num = (int*)HmmAlloc(sizeof(int));
    if (num == NULL) 
    {
        printf("Failed to allocate memory for num\n");
        return 1;
    }
    *num = 42;
    printf("Allocated num: %d\n\n\n", *num);



    char* str = (char*)HmmAlloc(20 * sizeof(char)); // should be 24  as alighment of 8
    if (str == NULL) 
    {
        printf("Failed to allocate memory for str\n");
        HmmFree(num);
        return 1;
    }




    char* str1 = (char*)HmmAlloc(33 * sizeof(char)); // should print 40 due to alighment of 8
    if (str1 == NULL) 
    {
        printf("Failed to allocate memory for str\n");
        HmmFree(num);
        return 1;
    }

    

    printf("Freeing num...\n");
    HmmFree(num);
    printf("Freed num\n\n\n");

    printf("Freeing str...\n");
    HmmFree(str);
    printf("Freed str\n\n\n");


    printf("Freeing str...\n");
    HmmFree(str1);
    printf("Freed str\n\n\n");


    // Optional call to rest heap, and to clear all its garbage data(For secure purposes)
    hmm_cleanup();

    return 0;
}