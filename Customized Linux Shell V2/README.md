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

## Features

### Built-in Commands

1. **mypwd**: Prints the current working directory.
   ```bash
   AliMamdouhShell > mypwd
   /home/ali/Task2_3_STM
   ```

2. **mycd**: Changes the current directory.
   ```bash
   AliMamdouhShell > mycd ../
   AliMamdouhShell > mypwd
   /home/ali
   ```

3. **myecho**: Outputs a string.
   ```bash
   AliMamdouhShell > myecho Hello, World!
   Hello, World!
   ```

4. **mytype**: Determines if a command is built-in or external.
   ```bash
   AliMamdouhShell > mytype mycd
   mycd is a shell built-in
   AliMamdouhShell > mytype /usr/bin/ls
   /usr/bin/ls is an external command
   ```

5. **myenvir**: Displays environment variables.
   ```bash
   AliMamdouhShell > myenvir
   SHELL=/bin/bash
   SESSION_MANAGER=local/ali-Victus-by-HP-Gaming-Laptop-15-fb0xxx:@/tmp/.ICE-unix/1377,unix/ali-Victus-by-HP-Gaming-Laptop-15-fb0xxx:/tmp/.ICE-unix/1377
   ...
   ```

6. **myphist**: Shows command history.
   ```bash
   AliMamdouhShell > myphist
   Last 5 commands:
   1: clear (exit status: 0)
   2: pwd (exit status: 0)
   3: help (exit status: 1)
   4: cd (exit status: 1)
   5: cp (exit status: 1)
   ```

7. **myhelp**: Lists available commands with brief descriptions.
   ```bash
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

### External Commands

The shell supports executing external commands available in the system's PATH:

```bash
AliMamdouhShell > ls
 commands.c   DebugOfTsk2      file2_WorkSpace.c   myshell.c                'strtok()_test_experiments.c'  'Task3 Version 1'
 commands.h   file1_WorkSpace.c   myshell         'Pictures for ReadMe'  'Task2 Version1'
```

### Error Handling

The shell provides error messages for unrecognized commands or execution failures:

```bash
AliMamdouhShell > nonexistentcommand
execvp error for nonexistentcommand: No such file or directory
```

## Notes

This shell implementation demonstrates basic functionality, error handling, system calls, process creation, and command execution in a Unix-like environment. It serves as a practical example of shell development and command processing.

```

This version provides a clearer and more structured presentation, ensuring that users understand how to compile, use, and troubleshoot the custom shell.
