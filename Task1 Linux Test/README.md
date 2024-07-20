![image](https://github.com/user-attachments/assets/693c10f7-7cbc-44ab-830b-24ed8d51adb5)# C Program Compilation and Analysis

This README provides instructions for compiling and analyzing a C program consisting of two files, along with various commands to examine the compilation process and output.

## Step 1: Create and Compile C Program

### Files:

- file1.c: Contains a function and one or more global variables used in this function.
- file2.c: Contains the main function and uses the function defined in file1.c.

### Compilation Command:

```bash

gcc -static -o my_program file1.c file2.c

```
![image](https://github.com/user-attachments/assets/ecfd39c1-2a5d-47e6-968c-5afb6aeacc5d)


## Step 2: Analyze Symbol Tables

Generate object files and examine their symbol tables, as well as the symbol table of the executable.

### Commands:

```bash

gcc -static -c file1.c -o file1.o

gcc -static -c file2.c -o file2.o

readelf -s file1.o

readelf -s file2.o

gcc -static -o my_program file1.c file2.c

readelf -s my_program

```

![image](https://github.com/user-attachments/assets/e08b111e-a717-4ee3-9536-152e8aa32db0)

![image](https://github.com/user-attachments/assets/f08e1b52-def9-436f-8098-c2cdb0de2d1c)




## Step 3: Analyze Section Sizes

Examine the sizes of .data and .text sections for object files and the executable using at least two different commands.

### Example Command:

```bash

size file1.o file2.o my_program

```

![image](https://github.com/user-attachments/assets/2a6823be-75d1-48f6-8cdb-afbe5d9ee2f9)


## Step 4: Generate Compilation Stage Files

Use a single command to compile a C file and generate all intermediate files (.i, .s, .o) as well as the executable.

### Commands:

```bash

gcc -save-temps -c file1.c

gcc -save-temps -c file2.c

```

## Step 5: Format and Display Section Sizes

Print the sizes of .text and .data sections for both object files and the executable in a specific format.

### Command:

```bash

echo -n "file1) "; size file1.o | tail -n 1 | awk '{printf "text: %d, data: %d\n", $1, $2}'
echo -n "file2) "; size file2.o | tail -n 1 | awk '{printf "text: %d, data: %d\n", $1, $2}'
echo -n "exec) "; size my_program | tail -n 1 | awk '{printf "text: %d, data: %d\n", $1, $2}'

```

![image](https://github.com/user-attachments/assets/d4d0d090-bfcc-48ce-8f29-d08dc8a3dece)


