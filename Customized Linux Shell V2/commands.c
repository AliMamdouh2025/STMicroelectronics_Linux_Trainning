/**
 *===================================================================================
 * @file           : commands.c
 * @author         : Ali Mamdouh
 * @brief          : Impelementation of commands to be executed by customized shell
 * @Reviwer        : Eng Kareem
 * @Version        : 2.2.0
 * @Company        : STMicroelectronics
 * @Date           : 30/7/2024
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
#include <stdbool.h>
#include <ctype.h>



/*============================================================================
 ********************************  Macros  ***********************************
 ============================================================================*/
#define STRINGS_ARE_EQUAL                  0
#define SUCCESS_OPERATION                  0
#define FAILED_OPERATION                  -1
#define FILE_NOT_EXIST                    -1
#define ALL_OPTIONS_ARE_PARSED            -1
#define INCREMENT_POINTER_BY_1             1
#define FILE_DESCRIPTOR_0                  0
#define FILE_DESCRIPTOR_1                  1
#define FILE_DESCRIPTOR_2                  2
#define ON                                 1
#define OFF                                0
#define IS_EXECUTABLE                      0
#define MAX_PATH                           4096



/*============================================================================
 **********************  Globaal Variables Definition  ***********************
 ============================================================================*/
struct ProcessInfo process_history[MAX_PROCESS_HISTORY];
int process_history_count = 0;



/*============================================================================
 ***********************  Functions Helper Definitions  **********************
 ============================================================================*/
 /**
 * Add a character at the beginning of a string.
 *
 * This function inserts a given character at the start of the provided string.
 * It shifts all existing characters in the string one position to the right to make room for the new character.
 * The function assumes that the string has enough space allocated to accommodate the additional character.
 *
 * @param str: Pointer to the null-terminated string (char*) where the character will be added.
 * @param c: The character (char) to add at the beginning of the string.
 */
void addCharAtBeginning(char *str, char c) 
{
    // Print error message if no Str is dangling pointer
    if(str == NULL)
    {
       perror("Error: Entering NULL to arguments of addCharAtBeginning() function");
       return;
    }
    
    // Get the length of the string
    size_t length = strlen(str);

    // Shift the characters in the string to the right
    for (size_t i = length; i > 0; i--) 
    {
        str[i] = str[i-1];
    }

    // Insert the new character at the beginning
    str[0] = c;

    // Null-terminate the string
    str[length + 1] = '\0';
    
}

 
 
 
 
 
 
/**
 * Parse and remove the '-a' option from the command-line arguments string.
 *
 * This function processes the provided command-line arguments string to identify and remove the '-a' option.
 * If the '-a' option is found, it updates the option pointer to include '-a' and removes '-a' from the original arguments string.
 * The function uses getopt() to parse the options and modifies the input string accordingly.
 *
 * @param args: Pointer to the command-line arguments string (char**). This string will be updated to exclude the '-a' option if found.
 * @param option: Pointer to the option string (char**). This will be set to "-a" if the option is found in the input arguments.
 */
void TakeAndRemove_Option(char **args, char **option, char *optionType) 
{

    // Print error message if one of arguments is not entered or have value of NULL
    if(*args == NULL || optionType == NULL)
    {
       perror("Error: Entering NULL to arguments of TakeAndRemove_Option() function");
       return;
    }
    
    // Declare variables for argument count, argument array, tokenization, and option parsing
    int argc = 0;
    char *argv[100];
    char *token;
    char optionArr[3];
    int opt;
    
    // copy in array so I can edit on it
    strcpy(optionArr, optionType);
    
    // Split the input string into an array of arguments
    // strdup() duplicates the input string so that it can be safely tokenized without modifying the original string.
    char *input = strdup(*args);
    
    // strtok() splits the input string into tokens based on spaces. The first token is assigned to 'token'.
    token = strtok(input, " ");
    
    // Loop through the tokens and store them in the argv array, incrementing argc for each token.
    while (token != NULL) 
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    // Reset getopt's internal state
    // optind is reset to 1 to ensure getopt() starts processing from the beginning of the argument list.
    optind = 1;
    
    // Initialize the option pointer to NULL
    *option = NULL;

    // Parse options using getopt()
    // getopt() parses the command-line arguments looking for the '-a' option.
    while ((opt = getopt(argc, argv, optionArr)) != ALL_OPTIONS_ARE_PARSED) 
    {
        // Process the option returned by getopt()
        if (opt == optionArr[0]) 
        {
             // If the '-a' option is found, duplicate the string "-a" and assign it to the option pointer.
             addCharAtBeginning(optionArr, '-'); // for eaxample from "a" to "-a" to pass oprion not character
             *option = strdup(optionArr); // duplicate to reserve original option
             break;
        }
    }
    
    // If no option was found, return early
    if (*option == NULL) return;

    // Create a new string excluding the '-a' option
    // Initialize an empty result string to build the new arguments string without the '-a' option.
    char result[1024] = "";
    
    // Loop through the argv array and concatenate each argument to the result string, excluding the '-a' option.
    for (int i = 0; i < argc; i++) 
    {
        if (i != optind - 1) 
        {
            strcat(result, argv[i]);
            strcat(result, " ");
        }
    }
    
    // Remove trailing space from the result string
    // If the result string is not empty and ends with a space, remove the trailing space by setting it to the null terminator.
    if (strlen(result) > 0 && result[strlen(result) - 1] == ' ') 
    {
        result[strlen(result) - 1] = '\0';
    }
    
    // Update the input args with the new string
    // Duplicate the result string and assign it to the args pointer.
    *args = strdup(result);

    // Free the duplicated input string
    free(input);
}

 
 
 
 
 
 
 
 
 
 
/**
 * Remove extra spaces from a file path string.
 *
 * This function processes the provided file path string to remove any leading, trailing, and extra spaces between words.
 * It ensures that there is only a single space between words if spaces are present.
 *
 * @param args: Pointer to the file path string (char*). This string will be modified to remove extra spaces.
 */
void RemovePathSpaces(char *args) 
{
    // Print error message if one of arguments is not entered or have value of NULL
    if(args == NULL)
    {
       perror("Error: Entering NULL to arguments of RemovePathSpaces() function");
       return;
    }

    // Declare variables for indexing and determining the length of the input string.
    int i = 0, j = 0;
    int len = strlen(args);

    // Remove leading spaces
    // Loop to skip over any leading space characters in the input string.
    while (isspace(args[i])) 
    {
        i++;
    }

    // Process the rest of the string
    // Iterate through the rest of the string starting from the first non-space character.
    for (; i < len; i++) 
    {
        // Copy non-space characters
        // If the current character is not a space, copy it to the next position in the modified string.
        if (!isspace(args[i])) 
        {
            args[j++] = args[i];
        } 
        else 
        {
            // If a space is found, check if the next character is non-space
            // This ensures that only single spaces are copied between words.
            if (i + 1 < len && !isspace(args[i + 1])) 
            {
                args[j++] = ' ';
            }
        }
    }

    // Remove trailing space if present
    // If the last character in the modified string is a space, remove it.
    if (j > 0 && isspace(args[j - 1])) 
    {
        j--;
    }

    // Null-terminate the string
    // Add the null terminator to the end of the modified string.
    args[j] = '\0';
}

 
 
 
 
 
 
 
 
 
 
 //The two paths must contains only one space between them without any other front or bachspaces,-
 // if not pass args first to RemovePathSpaces(char *args) function
 void split_Paths(char *args, char **source, char **dest) 
 {
    // Allocate memory for source and dest
    *source = (char *)malloc(strlen(args) + 1);
    *dest = (char *)malloc(strlen(args) + 1);
    
    // Print error message if one of arguments is not entered or have value of NULL
    if(*source == NULL || *dest == NULL)
    {
       perror("Error: memory allocation failed in split_Paths() function");
       return;
    }

    // Initialize pointers for parsing
    char *start = args;
    char *end;

    // Check if the first path is quoted
    if (*start == '"') 
    {
        // Find the end quote of the first path
        end = strchr(start + 1, '"');
        if (end == NULL) 
        {
            // Invalid input
            return;
        }
 
        // Copy the first path (including quotes)
        strncpy(*source, start, end - start + 1);
        (*source)[end - start + 1] = '\0';
        
        // Skip to the second path
        start = end + 2; // Skip the quote and the space
    } else 
    {
        // Find the space separating the two paths
        end = strchr(start, ' ');
        if (end == NULL) 
        {
            // Invalid input
            return;
        }
        // Copy the first path
        strncpy(*source, start, end - start);
        (*source)[end - start] = '\0';
        // Skip to the second path
        start = end + 1; // Skip the space
    }

    // Copy the second path
    strcpy(*dest, start);
}

 
 
 
 
 

/**
 * Add a command and its exit status to the process history.
 *
 * This function stores the provided command and its exit status in the process history array.
 * The process history operates as a circular buffer, with a maximum size defined by MAX_PROCESS_HISTORY.
 * When the number of stored commands exceeds this size, the oldest entries are overwritten.
 *
 * The function uses strncpy() to safely copy the command string into the process history buffer,
 * ensuring it does not exceed the buffer size and is null-terminated.
 *
 * @param command: Pointer to the command string (const char*). This is the command to be stored in the process history.
 * @param exit_status: Integer exit status of the command. This status is stored along with the command in the process history.
 */
void add_to_process_history(const char *command, int exit_status) 
{
        // Print error message if one of arguments is not entered or have value of NULL
        if(command == NULL)
        {
           perror("Error: Entering NULL command to add_to_process_history() function");
           return;
        }
        
        // create needed index
	int index = process_history_count % MAX_PROCESS_HISTORY;

	// Copy the command to the process history buffer
	// strncpy() copies up to sizeof(process_history[index].command) - 1 characters from the command,
	// ensuring the destination buffer is not overflowed and the string is null-terminated.
	strncpy(process_history[index].command, command, sizeof(process_history[index].command) - 1);
	process_history[index].command[sizeof(process_history[index].command) - 1] = '\0';

	// Store the exit status in the process history
	process_history[index].exit_status = exit_status;

	// Increment the process history count
	process_history_count++;
}








/**
 * This Function Splits a string of Paths into two strings of Paths, The Split begin from the first space-
 * where the first  Path is before the space and second Path is after the space.
 * 
 * It is a must that the first Path contains at its end file extension or this function will not work.
 *
 * It is a must to pass refrence of a pointer to the the twe pointers to pointers, So that the orginal two pointers changed-
 * by refrence and point to the new memory allocated
 */ 
void tokenize_Paths(const char *MainArgument, char **FirstPath, char **SecondPath) 
{
	// Allocate memory for B and C
	// We add this one as if no split happens and there is a strcat() occur inside below loop it will exceed size of MainArgument by one byte.
	*FirstPath = (char *)malloc(strlen(MainArgument) + 1); 
	*SecondPath = (char *)malloc(strlen(MainArgument) + 1);

	// Make sure memory is allocated correctly
	if (*FirstPath == NULL || *SecondPath == NULL) 
	{
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}

	// Initialize FirstPath and SecondPath
	(*FirstPath)[0] = '\0';
	(*SecondPath)[0] = '\0';

	// Tokenize the string based on spaces
	// Tokenize from a copy of MainArgument
	const char *delimiter = " ";
	char *needlePointer = NULL;
	char *token = strtok(strdup(MainArgument), delimiter); 

	// flag that remains true until first Path is constructed correctly
	int is_part_of_FirstPath = ON;


	// Loop on the full Path until iterating on all its elements(token == NULL)
	while (token != NULL) 
	{
		if (is_part_of_FirstPath) 
		{
			//Construct first path to the first half od original Path message
			strcat(*FirstPath, token);
			strcat(*FirstPath, " ");

			//return NULL if not found '.' and return a pointer points to '.' location if found.
			//strstr search for string and return a pointer to it, you may also use strchr
			needlePointer = strstr(token, ".");  

			//Check if first path contains extension of a file.               
			if ((needlePointer != NULL) && isalpha(*(needlePointer + 1)) && *(needlePointer + 2) == '\0')
			{
				is_part_of_FirstPath = OFF;
			}
		} else 
		{
			strcat(*SecondPath, token);
			strcat(*SecondPath, " ");
		}

		// Tokenize the next part of main argument
		token = strtok(NULL, delimiter);
	}

	// Remove trailing spaces
	(*FirstPath)[strlen(*FirstPath) - 1] = '\0';
	(*SecondPath)[strlen(*SecondPath) - 1] = '\0';
}








/** Add this function to handle quoted argument
 * 
 * This Function Filter Qutions of only One String(Ended With NULL Termination)
 *
 * If there are more than one argument in quoted String, You must seperate each argument in-
 * a seperated String then enter each String to this function to filter Qutions.
 *
 * For Exmaple this is one string: "/home/ali/Work Space STM Test/file1_WorkSpace.c"  "/home/ali/Task2_STM",-
 * You must seperate each of the two arguments in a seperate Strings then Pass each String to this function to filter quotions
 */
char* extract_quoted_arg(char* input) 
{
	if(input == NULL) return NULL;

	char* start = input;
	char* end;

	// Check if the first character is a quote
	if (*start == '"') 
	{
		start++;  // Move past the opening quote
		end = strchr(start, '"');
		if (end) 
		{
			*end = '\0';  // Terminate the string at the closing quote  
			strcat(start, (end + INCREMENT_POINTER_BY_1)); // Concatenate second half of string to its first half
			return start;
		}

	}

	// If the input doesn't start with a quote, return the whole input
	return input;
}




/*============================================================================
 **********************  Functions Commands Definitions  *********************
 ============================================================================*/

/**
 * Print the history of the last executed processes.
 *
 * This function prints a list of the most recent commands executed along with their exit statuses.
 * The number of processes shown is limited by MAX_PROCESS_HISTORY. If the number of executed
 * processes exceeds MAX_PROCESS_HISTORY, only the last MAX_PROCESS_HISTORY commands are displayed.
 *
 * The function calculates the starting index based on the current count of process history and
 * iterates through the history to print each command and its exit status.
 */
void cmd_phist(void) 
{
	int start = (process_history_count > MAX_PROCESS_HISTORY) ? 
			(process_history_count - MAX_PROCESS_HISTORY) : 0;

	int count = (process_history_count > MAX_PROCESS_HISTORY) ? 
			MAX_PROCESS_HISTORY : process_history_count;


	printf("Last %d processes:\n", count);
	for (int i = 0; i < count; i++) 
	{
		int index = (start + i) % MAX_PROCESS_HISTORY;
		printf("%d: %s (exit status: %d)\n", i + 1, 
				process_history[index].command, 
				process_history[index].exit_status);
	}
}






/**
 * Change the current working directory.
 *
 * This function attempts to change the current working directory to the specified path.
 * If the path is NULL or the directory change fails, an error message is printed.
 *
 * @param path: Pointer to the path string (char*). This is the directory path to which
 *              the current working directory should be changed.
 */
void cmd_cd(char *path) 
{
	if (path == NULL) 
	{
		fprintf(stderr, "Usage: cd <path>\n");
		return;
	}
	if (chdir(path) != 0) 
	{
		perror("cd");
	}
}






/**
 * Determine if a command is a shell built-in or an external command.
 *
 * This function checks if the specified command is a shell built-in command or an external command.
 * It prints a message indicating the type of the command.
 *
 * If the command is not a recognized built-in command and is not found in the system PATH,
 * a message stating that the command is not recognized is printed.
 *
 *
 * @param command: Pointer to the command string (char*).
 */
void cmd_type(char *command) 
{
    // Check if the provided command is NULL
    if (command == NULL) 
    {
        // Print usage information to standard error if no command is provided
        fprintf(stderr, "Usage: type <command>\n");  
              
        // Return from the function as there's nothing to process
        return;
    }
    

    // Check if the command matches any predefined shell built-in commands
    if (strcmp(command, "mycp") == 0 || strcmp(command, "mymv") == 0 ||
        strcmp(command, "mypwd") == 0 || strcmp(command, "myecho") == 0 ||
        strcmp(command, "myhelp") == 0 || strcmp(command, "myexit") == 0 ||
        strcmp(command, "mycd") == 0 || strcmp(command, "mytype") == 0 ||
        strcmp(command, "myenvir") == 0 || strcmp(command, "myphist") == 0) 
    {
        // If the command is a shell built-in, print this information
        printf("%s is a shell built-in\n", command);        
        
        // Return from the function as the command type is determined
        return;
    }
    

    // Retrieve the PATH environment variable which contains directories to search for external commands
    char *path = getenv("PATH");
    
    // Check if the PATH environment variable is not set
    if (path == NULL) 
    {
        // Print an error message if PATH is not set
        fprintf(stderr, "Error: PATH environment variable not set\n");
        
        // Return from the function as we cannot proceed without PATH
        return;
    }
    

    // Create a duplicate of the PATH string to avoid modifying the original PATH
    char *path_copy = strdup(path);
    
    
    // Tokenize the duplicated PATH string using colon as the delimiter to get each directory
    char *dir = strtok(path_copy, ":");
    

    // Iterate over each directory in the PATH
    while (dir != NULL) 
    {
        // Create a buffer to store the full path of the command
        char full_path[MAX_PATH];
        
        // Construct the full path by combining the directory and the command
        // Check if constructed Path is too long
        int ret = snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
        
        // if constructed Path is to long print error
        if (ret >= sizeof(full_path)) 
        {
            // Truncation occurred
            fprintf(stderr, "Warning: very long command path, bigger than MAX_PATH\n");
        }        

        // Check if the constructed full path is an executable file
        // X_OK is the mode used to check if the file specified by full_path has execute permissions.
        if (access(full_path, X_OK) == IS_EXECUTABLE) 
        {
            // If the command is executable, print this information
            printf("%s is an external command\n", command);
            
            // Free the memory allocated for path_copy
            free(path_copy);
            
            // Return from the function as the command type is determined
            return;
        }

        // Move to the next directory in the PATH
        dir = strtok(NULL, ":");
    }
    

    // Free the memory allocated for path_copy after all directories have been checked
    free(path_copy);
    

    // Check if the command is executable in the current directory OR-
    // execute command if user enter full path
    if (access(command, X_OK) == 0) 
    {
        // If the command is executable, print this information
        printf("%s is an external command\n", command);
    } 
    else 
    {
        // If the command is not found anywhere, print this information
        printf("%s is not recognized as an internal or external command\n", command);
    }
}






/**
 * Print environment variables or a specific environment variable.
 *
 * This function prints all environment variables if no specific variable is provided.
 * If a variable name is given, it prints the value of that specific environment variable.
 * If the specified environment variable is not found, an error message is printed.
 *
 * @param variable: Pointer to the variable name string (char*). This is the name of the environment
 *                  variable to be printed. If NULL, all environment variables are printed.
 */
void cmd_envir(char *variable) 
{
	if (variable == NULL) 
	{
		// Print all environment variables
		extern char **environ;
		for (char **env = environ; *env != NULL; env++) 
		{
			printf("%s\n", *env);
		}
	} else 
	{
		// Print specific environment variable
		char *value = getenv(variable);
		if (value != NULL) 
		{
			printf("%s=%s\n", variable, value);
		} else 
		{
			fprintf(stderr, "Environment variable %s not found\n", variable);
		}
	}
}








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
        // Print error message if argument is not entered or have value of NULL
        if(args == NULL)
        {
           // Prints Enter if there is no arguments to be like original shell
           write(FILE_DESCRIPTOR_1, "\n", 1);
           return;
        }
        
        // Filter quotes if it exists
        args =  extract_quoted_arg(args);
        
        // Writing contents of args then write enter to file descrptor1(output terminal) 
        write(FILE_DESCRIPTOR_1, args, strlen(args));
        write(FILE_DESCRIPTOR_1, "\n", 1);
}











/**
 * Copy the contents of the source file to the destination file.
 * 
 * This function copies the content from the source file to the destination file. It supports an optional `-a` flag,
 * which enables append mode. If the destination file is the same as the source file, the function prints an error unless 
 * the `-a` flag is used. If the destination is a directory, the function copies the source file (with its same name) 
 * to the destination path.
 * 
 * The function performs error checking to ensure that both source and destination are specified, and handles file opening,
 * reading, and writing operations.
 *
 * @param args: Pointer to a null-terminated string containing the command-line arguments.
 */
void cmd_mycp(char *args)
{
    // Print error if no arguments are entered
    if(args == NULL)
    {
        perror("Error: No arguments passed to mycp command!!");
        return;       
    }

    // Declare pointers for source, destination, and optional flag
    char *source = NULL;
    char *dest = NULL;
    char *option = NULL;
    
    // Extract the optional flag from the arguments
    TakeAndRemove_Option(&args, &option, "a");
    
    // Remove extra spaces from the arguments
    RemovePathSpaces(args);

    // Split the arguments into source and destination paths
    split_Paths(args, &source, &dest);

    // Remove quotes from the source and destination paths
    source = extract_quoted_arg(source);
    dest = extract_quoted_arg(dest);

    // Initialize the append mode flag to false
    bool append = false;

    // Check if source and destination are specified
    if (!source || !dest) 
    {
        fprintf(stderr, "Usage: cp [-a] source destination\n");
        return;
    }

    // Check for append mode flag and set the append variable
    if (option && strcmp(option, "-a") == STRINGS_ARE_EQUAL) 
    {
        append = true;
    }

    // Convert relative paths to absolute paths
    char abs_source[MAX_PATH];
    char abs_dest[MAX_PATH];
    
    source = realpath(source, abs_source);
    if (source == NULL) 
    {
        perror("Error resolving source path");
        return;
    }
    

    dest = realpath(dest, abs_dest);
    if (dest == NULL) 
    {
        perror("Error resolving destination path");
        return;
    }

    // Declare and initialize a stat structure to store information about the destination file
    struct stat st = {0};

    // Check if the destination is a directory
    if (stat(dest, &st) == SUCCESS_OPERATION && S_ISDIR(st.st_mode)) 
    {
        // Extract the base name of the source file from the path
        char *base = basename(strdup(source));
        
        // Buffer to construct new destination path
        char new_dest[MAX_PATH];
        
        // Construct new destination path
        snprintf(new_dest, sizeof(new_dest), "%s/%s", dest, base);
        
        // Update destination to the new path
        dest = new_dest;
    }        

    // Check if the destination file exists (only in non-append mode)
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

    // Open the destination file for writing in truncate (overwrite) or append mode, create if not exists
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
void cmd_mymv(char *args) 
{

         // Print error if no arguments are entered
         if(args == NULL)
         {
            perror("Error: No arguments passed to mymv command!!");
            return;       
         }

	// Tokenize the arguments to extract source, destination, and optional flag
	char *source = NULL;
	char *dest = NULL;
	char *option = NULL;

        // Extract the optional flag from the arguments
        TakeAndRemove_Option(&args, &option, "f");
    
        // Remove extra spaces from the arguments
        RemovePathSpaces(args);

        // Split the arguments into source and destination paths
        split_Paths(args, &source, &dest);

        // Remove quotes from the source and destination paths
        source = extract_quoted_arg(source);
        dest = extract_quoted_arg(dest);

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
	
        // Convert relative paths to absolute paths
        char abs_source[MAX_PATH];
        char abs_dest[MAX_PATH];
    
        source = realpath(source, abs_source);
        if (source == NULL) 
        {
            perror("Error resolving source path");
            return;
        }


        dest = realpath(dest, abs_dest);
        if (dest == NULL) 
        {
           perror("Error resolving destination path");
           return;
        }


	// Initialize a stat structure to store information about the destination
	struct stat st = {0};

	// Check if the destination path is a directory
	if (stat(dest, &st) == SUCCESS_OPERATION && S_ISDIR(st.st_mode)) 
	{
		// Extract the base name of the source file (e.g., "file.txt" from "/path/to/file.txt")
		char *base = basename(strdup(source));

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
 */
void cmd_help(void) 
{
	printf("Supported builtin commands are:\n");
	printf("1- mypwd : print working directory\n");
	printf("2- myecho : print a user input string on stdout\n");
	printf("3- mycp [-a] : copy a file to another file (use -a to append)\n");
	printf("4- mymv [-f] : move a file to another place (use -f to force overwrite)\n");
	printf("5- myexit : terminate the shell\n");
	printf("6- myhelp : print all supported commands with brief info\n");
	printf("7- mycd : change the current directory\n");
	printf("8- mytype : return the type of the command\n");
	printf("9- myenvir : print environment variables\n");
	printf("10- myphist : list the last 10 processes with their exit status\n");
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






// printf("Inside Function DestFile: %s\n", dest);  ////////////////////////////////////////////////////////////////////////////// Debug print
