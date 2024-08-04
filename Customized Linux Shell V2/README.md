# Custom Shell Implementation Version 2

This project provides a custom shell that includes built-in commands and the capability to execute external commands.

## Compilation

To compile the custom shell, use the following command:

```
gcc -o myshell myshell.c commands.c -lreadline
```

Ensure that the `readline` library is installed on your system. On Ubuntu or Debian-based systems, you can install it with:

```
sudo apt-get install libreadline-dev
```

## Usage

After compilation, start the shell with:

```bash
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


