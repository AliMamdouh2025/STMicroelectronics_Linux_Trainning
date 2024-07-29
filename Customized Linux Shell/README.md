# Custom Shell Implementation

This project implements a custom shell with various command-line functionalities. All shell commands handles spaces before, between, and after arguments.
The shell is built and executed using the following steps:

## Building and Running the Shell

1. Compile the project by linking the necessary readline library:
   ```
   gcc -o myshell myshell.c commands.c -lreadline
   ```

2. Run the shell:
   ```
   ./myshell
   ```
![Step1 execute](https://github.com/user-attachments/assets/4d9575cd-5609-413b-8a56-495854638c88)

## Features

### 1. PWD Command
Prints the current working directory.

Example:
```
AliMamdouhShell > pwd
/home/user/current/directory
```
![Feature1 pwd](https://github.com/user-attachments/assets/df28a009-501e-4cf4-ab68-26f008d1c7e7)

### 2. CP Command
Copies files with various options:

- Basic copy:
  ```
  AliMamdouhShell > cp /home/ali/test_1/file1.c /home/ali/Task2_STM
  ```
![Feature1 cp test1](https://github.com/user-attachments/assets/571f2aae-bef4-4dd3-90c8-a34c1e926a2b)

- Copy with a new name:
  ```
  AliMamdouhShell > cp /home/ali/test_1/file1.c /home/ali/Task2_STM/Copied_file1.c
  ```
![Feature1 cp test2](https://github.com/user-attachments/assets/8cc8b6f7-93e1-4054-91ec-33ef0c049e16)

- Handling existing files:
  ```
  AliMamdouhShell > cp /home/ali/test_1/file1.c /home/ali/Task2_STM/Copied_file1.c
  Error: Destination file already exists
  ```
![Feature1 cp test3](https://github.com/user-attachments/assets/7ba42246-d884-417e-b440-85bcaa68f3bc)

- Append option:
  ```
  AliMamdouhShell > cp /home/ali/test_1/file1.c /home/ali/Task2_STM/Copied_file1.c -a
  ```
![Feature1 cp test4](https://github.com/user-attachments/assets/3dd9a91d-818b-4320-bee8-bf35760a53cb)



### 3. MV Command
Moves or renames files:

- Move to a directory:
  ```
  AliMamdouhShell > mv /home/ali/test_1/file2.c /home/ali/Task2_STM
  ```
![Feature2 mv test1](https://github.com/user-attachments/assets/c2a7ce1c-c96e-4756-bf92-57380b33afbc)

- Move with a new name:
  ```
  AliMamdouhShell > mv /home/ali/test_1/file3.c /home/ali/Task2_STM/Moved_File3.c
  ```
![Feature2 mv test2](https://github.com/user-attachments/assets/2ea52a85-ce75-43ee-b767-3bf8f542b1f1)

- Handling existing files:
  ```
  AliMamdouhShell > mv /home/ali/test_1/file4.c /home/ali/Task2_STM/Moved_File3.c
  Error: Destination file already exists
  ```
![Feature2 mv test3](https://github.com/user-attachments/assets/10d6759f-1296-4717-abab-f975ec846570)

- Force overwrite:
  ```
  AliMamdouhShell > mv /home/ali/test_1/file4.c /home/ali/Task2_STM/Moved_File3.c -f
  ```
![Feature2 mv test4](https://github.com/user-attachments/assets/0fd01274-5a61-4419-aa73-9496401a4a65)

### 4. Echo Command
Prints the specified text to the console.

Example:
```
AliMamdouhShell > echo Hello Ali
Hello Ali
```
![Feature3 echo](https://github.com/user-attachments/assets/150f8824-363c-4f1f-8942-2a46e9f1db2c)

### 5. Help Command
Displays all supported commands with brief descriptions.
![Feature4 help](https://github.com/user-attachments/assets/ccc6ec1b-fd27-43a0-addb-da1deb2d2088)

Example:
```
AliMamdouhShell > help
Supported commands are:
1- pwd : print working directory
2- echo : print a user input string on stdout
3- cp [-a] : copy a file to another file (use -a to append)
4- mv [-f] : move a file to another place (use -f to force overwrite)
5- exit : terminate the shell
6- help : print all supported commands with brief info
```

### 6. Command History
Use the up/down arrow keys to navigate through previously entered commands.

### 7. Exit Command
Terminates the shell session.

Example:
```
AliMamdouhShell > exit
Good Bye :)
```
![Feature6 exit](https://github.com/user-attachments/assets/27029a7d-1ec2-42b2-8cf9-769a30f8acf9)

## Error Handling

The shell includes robust error handling for various scenarios like:
- Incorrect command syntax
- Non-existent files or directories
- Missing command arguments
- Attempts to overwrite existing files without proper flags
- Can handle directories which contain spaces in its names
- Filter quutions of the echo to be exactly like echo shell system Response

Example:

![Error Handling](https://github.com/user-attachments/assets/1c551207-2644-432f-a15e-7262008d1fad)

![Error Handling2](https://github.com/user-attachments/assets/fa2eb207-c792-4769-8472-128f770b73d7)

![Error Handling3](https://github.com/user-attachments/assets/e54274ec-8a3e-46a2-93f8-f01c728340bb)


