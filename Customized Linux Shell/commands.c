/**
 *===================================================================================
 * @file           : commands.c
 * @author         : Ali Mamdouh
 * @brief          : Impelementation of commands to be executed bu customized shell
 *===================================================================================
 * 
 *===================================================================================
 */



/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <getopt.h>
#include "commands.h"



/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/
#define MAX_PATH                     4096
#define BUFFER_SIZE                  4096




/*============================================================================
 ********************************  Macros  ***********************************
 ============================================================================*/
#define STRINGS_ARE_EQUAL             0
#define SUCCESS_OPERATION             0
#define FAILED_OPERATION             -1
#define FILE_NOT_EXIST               -1




/*============================================================================
 **************************  Functions Definitions  **************************
 ============================================================================*/
/**
 * Print the current working directory (CWD).
 *   
 * If successful, the directory path is printed to the standard output.
 * 
 * If an error occurs, an error message is printed.
 */
void cmd_pwd(void) 
{
	// Buffer to store the current working directory path
	char cwd[MAX_PATH];

	// Calls the getcwd function, which attempts to retrieve the current working directory path 
	// and store it in the cwd buffer.
	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
		// If successful, print the current working directory path
		printf("%s\n", cwd);
	} 
	else 
	{
		// If an error occurs, print an error message
		perror("getcwd() error");
	}
}






/**
 * Print the given arguments to the standard output.
 *
 * @param args: Pointer to a null-terminated string containing the arguments to be printed.
 */
void cmd_echo(char *args) 
{
	// Print the provided arguments followed by a newline
	printf("%s\n", args);
}






/**
 * Copy the contents of the source file to the destination file.
 * 
 * If destination file is the same of source file the function print error
 *
 * It supports an optional `-a` flag, which enables append mode-
 * even if destination file is the same of source file. 
 * 
 * If the destination is a directory, the function copy the source file(with its same name)-
 * to the destination path.
 * 
 * The function performs error checking to ensure that both source and destination-
 * are specified and handles file opening, reading, and writing operations.
 *
 * @param args: Pointer to a null-terminated string containing the command-line arguments.
 */
void cmd_cp(char *args) 
{
    // Tokenize the arguments to extract source, destination, and optional flag
    char *source = strtok(args, " ");
    char *dest = strtok(NULL, " ");
    char *option = strtok(NULL, " ");
    
    // Initializes a boolean variable append to false. 
    bool append = false;

    // Check if source and destination are specified
    if (!source || !dest) 
    {
        fprintf(stderr, "Usage: cp [-a] source destination\n");
        return;
    }

    // Check for append mode flag
    if (option && strcmp(option, "-a") == STRINGS_ARE_EQUAL) 
    {
        append = true;
    }

    //Declares and initializes a stat structure to store information about the destination file.
    struct stat st = {0};
    
    // Check if destination is directory
    // It fills the stat structure (st) with details about the file. 
    // The stat structure contains various fields, including st_mode, which describes the file type and permissions-
    // If successful, stat returns 0. If it returns -1, it indicates an error occurred.
    // S_ISDIR(st.st_mode): A macro that checks if the st_mode field in the stat structure indicates that dest is a directory
    if (stat(dest, &st) == SUCCESS_OPERATION && S_ISDIR(st.st_mode)) 
    {
        char *base = basename(source);  // Extract the base name of the source file from the PATH
        char new_dest[MAX_PATH];  // Buffer to construct new destination path
        snprintf(new_dest, sizeof(new_dest), "%s/%s", dest, base);  // Construct new destination path
        dest = new_dest;  // Update destination to the new path
    }

    // Check if destination file exists(Check only in non-append mode)
    // The access function checks the existence of a file. 
    // The F_OK flag is used to check if the file exists, regardless of the type of access (read/write).
    if (!append && access(dest, F_OK) != FILE_NOT_EXIST) 
    {
        fprintf(stderr, "Error: Destination file already exists\n");
        return;
    }

    // Open the source file for reading
    int src_fd = open(source, O_RDONLY);
    if (src_fd == FAILED_OPERATION) 
    {
        perror("Error opening source file");
        return;
    }

    // Open the destination file for writing in truncate(overwrite) or append mode, Create if not exists. 
    // 0644: This is the file permission modewhen creating the file. It is expressed in octal format-
    // 6: Owner (user) has read and write permissions.
    // 4: Group has read permissions.
    // 4: Others have read permissions
    int dest_fd = open(dest, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
    if (dest_fd == FAILED_OPERATION) 
    {
        perror("Error opening destination file");
        close(src_fd);
        return;
    }

    // Buffer for file read/write operations
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    // Read from source and write to destination
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) 
    {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) 
        {
            perror("Write error");
            break;
        }
    }

    // Check for read error
    if (bytes_read == FAILED_OPERATION) 
    {
        perror("Read error");
    }

    // Close source and destination files
    close(src_fd);
    close(dest_fd);
}






/**
 * Move or rename a file from a source path to a destination path.
 *  
 * It supports an optional flag `-f` (force) to overwrite the destination file if it already exists.
 * 
 * It also checks if the destination is a directory, in which case the file will be moved inside the directory-
 * with its base name preserved. If the file operation fails, appropriate error messages are printed.
 *
 * @param args: A string containing the arguments for the move operation. The string is expected to be in the format:
 *              "source destination [-f]". The `source` is the path of the file to be moved, and `destination` 
 *              is the target path. The optional `-f` flag forces the operation to overwrite the destination if it exists.
 */
void cmd_mv(char *args) 
{
    // Extract the source file path from the arguments
    char *source = strtok(args, " ");
    
    // Extract the destination path from the arguments
    char *dest = strtok(NULL, " ");
    
    // Extract the optional flag or any additional arguments
    char *option = strtok(NULL, " ");
    
    // Boolean flag to determine whether to forcefully overwrite an existing destination file
    bool force = false;

    // Check if both source and destination are provided
    if (!source || !dest) 
    {
        // Print usage information if the required arguments are missing
        fprintf(stderr, "Usage: mv [-f] source destination\n");
        return;
    }

    // Check if the optional force flag is set
    if (option && strcmp(option, "-f") == STRINGS_ARE_EQUAL) 
    {
        force = true;
    }

    // Initialize a stat structure to store information about the destination
    struct stat st = {0};
    
    // Check if the destination path is a directory
    if (stat(dest, &st) == SUCCESS_OPERATION && S_ISDIR(st.st_mode)) 
    {
        // Extract the base name of the source file (e.g., "file.txt" from "/path/to/file.txt")
        char *base = basename(source);
        
        // Buffer to construct the new destination path
        char new_dest[MAX_PATH];
        
        // Construct the new destination path by appending the base name to the destination directory
        snprintf(new_dest, sizeof(new_dest), "%s/%s", dest, base);
        
        // Update the destination to the newly constructed path
        dest = new_dest;
    }

    // Check if the destination file exists and the force flag is not set
    if (!force && access(dest, F_OK) != FILE_NOT_EXIST) 
    {
        // Print an error message if the file exists and overwriting is not allowed
        fprintf(stderr, "Error: Destination file already exists\n");
        return;
    }

    // Rename (Move) the source file to the destination path
    if (rename(source, dest) == FAILED_OPERATION) 
    {
        // Print an error message if the rename operation fails
        perror("Error moving file");
    }
}







/**
 * Display a help message listing all supported commands.
 *
 * This function provides a summary of the commands available in the shell. 
 * 
 * Each command is briefly described including its purpose and any optional flags-
 * or arguments it supports.
 *
 * Commands listed as follows:
 * - `pwd`: Prints the current working directory.
 * - `echo`: Prints a specified user input string to the standard output.
 * - `cp [-a]`: Copies a file to another file; the `-a` flag appends to the destination file if it exists.
 * - `mv [-f]`: Moves a file to a new location; the `-f` flag forces overwriting of the destination file if it exists.
 * - `exit`: Terminates the shell session.
 * - `help`: Displays this help message.
 */
void cmd_help(void) 
{
	printf("Supported commands are:\n");
	printf("1- pwd : print working directory\n");
	printf("2- echo : print a user input string on stdout\n");
	printf("3- cp [-a] : copy a file to another file (use -a to append)\n");
	printf("4- mv [-f] : move a file to another place (use -f to force overwrite)\n");
	printf("5- exit : terminate the shell\n");
	printf("6- help : print all supported commands with brief info\n");
}






/**
 * Terminate the shell session.
 *  
 *
 * @return: Returns `true` to indicate that the shell should exit. This value is used to update the `should_exit`
 *          flag in the main loop, thereby ending the shell session.
 */
bool cmd_exit(void) 
{
	printf("Good Bye :)\n");
	return true; 
}