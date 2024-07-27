// file1.c
#include <stdio.h>

int global_var1 = 10;
int global_var2;

void myFunction() {
    printf("Global variables: %d, %d\n", global_var1, global_var2);
}

