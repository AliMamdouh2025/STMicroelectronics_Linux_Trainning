# C Program Compilation and Analysis

This README provides instructions for compiling and analyzing a C program consisting of two files, along with various commands to examine the compilation process and output.

## Task 1: Create and Compile C Program

### Files:

- file1.c: Contains a function and one or more global variables used in this function.

- file1.h: Header file for file1.c.

- file2.c: Contains the main function and uses the function defined in file1.c.

### Compilation Command:

```bash

gcc -static -o my_program file1.c file2.c

```

## Task 2: Analyze Symbol Tables

Generate object files and examine their symbol tables, as well as the symbol table of the executable.

### Commands:

```bash

gcc -static -c file1.c -o file1.o

gcc -static -c file2.c -o file2.o

```

## Task 3: Analyze Section Sizes

Examine the sizes of .data and .text sections for object files and the executable using at least two different commands.

### Example Command:

```bash

size file1.o file2.o my_program

```

## Task 4: Generate Compilation Stage Files

Use a single command to compile a C file and generate all intermediate files (.i, .s, .o) as well as the executable.

### Commands:

```bash

gcc -save-temps -c file1.c

gcc -save-temps -c file2.c

```

## Task 5: Format and Display Section Sizes

Print the sizes of .text and .data sections for both object files and the executable in a specific format.

### Command:

```bash

size file1.o file2.o Session2_Task1 | awk '

NR==2 {print "file1) text: " $1 ", data: " $2}

NR==3 {print "file2) text: " $1 ", data: " $2}

NR==4 {print "exec)  text: " $1 ", data: " $2}'

```

