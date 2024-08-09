# Custom Shell Implementation Version 2

This project provides a custom shell that includes built-in commands and the capability to execute external commands.

## Compilation

To compile the custom shell, use the following command:

```
gcc -o myshell myshell.c commands.c variables.c -lreadline -lm```
```

Ensure that the `readline` library is installed on your system. On Ubuntu or Debian-based systems, you can install it with:
sudo apt-get install libreadline-dev

## Usage
After compilation, start the shell with:

```
./myshell
```

## Added Features

### Built-in Commands
1. **myphist**: Shows command history.
   ```
   AliMamdouhShell > myphist
   Last 5 commands:
   1: clear (exit status: 0)
   2: pwd (exit status: 0)
   3: help (exit status: 1)
   4: cd (exit status: 1)
   5: cp (exit status: 1)
   ```
![image_2024-07-30_08-54-41](https://github.com/user-attachments/assets/85958c3b-7cbf-4807-bd40-92d99ee42688)

**Data Structure Used:** Circular buffer, Which efficiently manages recent command history by using a fixed-size buffer with constant-time insertions and retrievals. This ensures fast performance and consistent memory usage to get the last process being used for **myphist** command. 
![CircularBufferBetter-transformed](https://github.com/user-attachments/assets/14ddac3b-b479-4810-b204-7e56b0c44872)



2. **mycd**: Changes the current directory.
   ```
   AliMamdouhShell > mycd ../
   AliMamdouhShell > mypwd
   /home/ali
   ```
![image_2024-07-30_08-40-52](https://github.com/user-attachments/assets/bc55edfa-e276-475b-acd6-9592969516e6)


3. **myecho**: Outputs a string.
   ```
   AliMamdouhShell > myecho Hello, World!
   Hello, World!
   ```
![image_2024-07-30_08-44-49](https://github.com/user-attachments/assets/86211aee-7eb9-4789-9cbc-b2b2a958f12c)



4. **mytype**: Determines if a command is built-in or external.
   ```
   AliMamdouhShell > mytype mycd
   mycd is a shell built-in
   AliMamdouhShell > mytype ls
   ls is an external command
   AliMamdouhShell > which ls
   /usr/bin/ls
   AliMamdouhShell > mytype /usr/bin/ls
   /usr/bin/ls is an external command
   AliMamdouhShell > mytype sads
   sads is not recognized as an internal or external command
   ```
![mytypeCornerCases](https://github.com/user-attachments/assets/39cfdd0e-82b6-46cd-aa01-ae6088453dc2)


5. **myenvir**: Displays environment variables.
   ```
   AliMamdouhShell > myenvir
   SHELL=/bin/bash
   SESSION_MANAGER=local/ali-Victus-by-HP-Gaming-Laptop-15-fb0xxx:@/tmp/.ICE-unix/1377,unix/ali-Victus-by-HP-Gaming-Laptop-15-fb0xxx:/tmp/.ICE-unix/1377
   ...
   ```
   ![image_2024-07-30_08-49-05](https://github.com/user-attachments/assets/b37fcd5c-31b5-4366-ae98-d997b2ecb483)



6. **myhelp**: Lists available commands with brief descriptions.
   ```
   AliMamdouhShell > myhelp
   Supported built-in commands:
   1. mypwd : Print working directory
   2. myecho : Echo a string
   3. mycp [-a] : Copy a file (use -a to append)
   4. mymv [-f] : Move a file (use -f to force overwrite)
   5. myexit : Exit the shell
   6. myhelp : List all commands
   7. mycd : Change directory
   8. mytype : Determine command type
   9. myenvir : Print environment variables
   10. myphist : Show recent commands with exit status
   ```
![image_2024-07-30_08-51-25](https://github.com/user-attachments/assets/a2c1c41e-3693-4362-81ab-27bbbf50fd53)

7. **mycp**: Improve the command so it can copy files using relative paths even if paths have spaces in its naming.
![CopyingTestCase](https://github.com/user-attachments/assets/75364054-8797-4dea-9458-3912fbf5c2f2)

![CopyingTestCase2](https://github.com/user-attachments/assets/e17d100e-8021-4f24-b064-b50e0bb0db1d)

![CopyingTestCase3](https://github.com/user-attachments/assets/83adbf15-73ef-4ad8-bfe3-2c06a53f4fe6)

![CopyingTestCase4](https://github.com/user-attachments/assets/0417bd83-3208-4b27-b184-05b44638ebe6)


8. **mymv**: Improve the command so it can move files using relative paths even if paths have spaces in its naming.

![CuttingTestCase](https://github.com/user-attachments/assets/4ad4da88-091e-4672-b0c8-3a76d88fe198)

![CuttingTestCase2](https://github.com/user-attachments/assets/664196b7-cc02-4a6d-8a75-5bfc2f5ac730)

![CuttingTestCase3](https://github.com/user-attachments/assets/09639de4-938d-4bb2-ae54-c2fa7ec24b4b)

![CuttingTestCase4](https://github.com/user-attachments/assets/e2791c88-791e-452b-9f39-4aeaca80aea5)





### External Commands

The shell supports executing external commands available in the system's PATH, For example:

```
AliMamdouhShell > ls
 commands.c   DebugOfTsk2      file2_WorkSpace.c   myshell.c                'strtok()_test_experiments.c'  'Task3 Version 1'
 commands.h   file1_WorkSpace.c   myshell         'Pictures for ReadMe'  'Task2 Version1'
```

### Error Handling

The shell provides error messages for unrecognized commands or execution failures:

```
AliMamdouhShell > nonexistentcommand
execvp error for nonexistentcommand: No such file or directory
```
![image_2024-07-30_08-58-54](https://github.com/user-attachments/assets/09984a12-47ef-47e5-b739-5d6fd1ab460b)







# GDB Script Example: Print Process History and Count

## Overview

This GDB script defines a custom function to print the process history and count from the `process_history` array and the `process_history_count` variable. It provides a clear, organized, and readable output to help in debugging and analyzing the process execution history.

## Script Details

The script includes a function `print_process_history` that:
1. Prints the value of the `process_history_count` variable.
2. Prints a table header for the process history.
3. Iterates through the `process_history` array and prints the index, command, and exit status of each process in a formatted manner.

Additionally, an alias `printd` is created for easier access to the function.

## Structure of `ProcessInfo`

The script assumes that the `ProcessInfo` structure is defined with at least the following members:
```c
struct ProcessInfo {
    char command[256];
    int exit_status;
};
```

### Steps to Use the Script

1. **Create the GDB Script File**:
   - Create a file named `print_process_history.gdb`.
   - Copy and paste the content of print_process_history.gdb into the file:


2. **Start GDB**:
   - Open a terminal and start GDB with your executable. For example:
     ```sh
     gdb ./your_program
     ```

3. **Load the Script in GDB**:
   - Within the GDB session, load the script using the `source` command:
     ```gdb
     source /path/to/print_process_history.gdb
     ```
   - Replace `/path/to/print_process_history.gdb` with the actual path to the script file.

4. **Run Your Program (if necessary)**:
   - If your program needs to be running to populate the `process_history` and `process_history_count`(which is our case) , start it with:
     ```gdb
     run
     ```
   - Ensure that the program has executed up to the point where the `process_history` and `process_history_count` are populated.

5. **Call the GDB Function**:
   - To print the process history and count, simply call the defined function:
     ```gdb
     print_process_history
     ```
   - Or use the alias for convenience:
     ```gdb
     printd
     ```



# GDB Video link
https://drive.google.com/file/d/1y408VEdQHFd3YLZ7NBrcBYNNYxwjZV83/view?usp=sharing
