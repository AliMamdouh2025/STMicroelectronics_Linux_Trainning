/**
 *===================================================================================
 * @file           : commands.c
 * @author         : Ali Mamdouh
 * @brief          : Impelementation of commands to be executed bu customized shell
 * @Reviwer        : Eng Kareem
 * @Version        : 3.0.0
 * @Company        : STMicroelectronics
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
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "commands.h"
#include "variables.h"



/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/
#define MAX_INPUT                               1024
#define PROMPT                                  "AliMamdouhShell > "
#define MAX_NUMBER_OF_ARGUMENTS                 64
#define MAX_REDIRECTIONS                        3
#define MAX_PIPES                               10
#define NO_PIPELINE                             1




/*============================================================================
 ********************************  Macros  ***********************************
 ============================================================================*/
#define INCREMENT_POINTER_BY_1                  1
#define STRINGS_ARE_EQUAL                       0
#define FORK_FAILED                            -1
#define PROCESS_FAILED                         -1
#define IS_CHILD                                0




/*============================================================================
 *************************  Data types Declerations  *************************
 ============================================================================*/
// Struct for commands history
typedef struct 
{
	const char *name;
	void (*function)(char*);
} Command;






// Struct for redirection
struct redirection 
{
	int type;  // 0 for input, 1 for output, 2 for error
	char *file;
};





// Struct for Pipelinning
struct command 
{
	char *argv[MAX_NUMBER_OF_ARGUMENTS];
	int argc;
	struct redirection redirections[MAX_REDIRECTIONS];
	int redirection_count;
};




/*============================================================================
 **********************  Globaal Variables Definition  ***********************
 ============================================================================*/
// Config List of internal commands, Made by this way to let the code more maintainable
Command internal_commands[] = 
{
		{"mycp", cmd_mycp},
		{"mymv", cmd_mymv},
		{"mypwd", (void (*)(char*))cmd_pwd},  // Cast to match function signature in the below iteration, This help the code to be more maintainable
		{"myecho", cmd_echo},
		{"myhelp", (void (*)(char*))cmd_help},
		{"mycd", cmd_cd},
		{"mytype", cmd_type},
		{"myenvir", cmd_envir},
		{"myphist", (void (*)(char*))cmd_phist},
		{"myfree", (void (*)(char*))cmd_free},
		{"myuptime", (void (*)(char*))cmd_uptime}, 
		{"myallVar", (void (*)(char*))cmd_allVar}, 	
		{NULL, NULL}  // This is used to mark the end of the array.
};




/*============================================================================
 **************************  Functions Definitions  **************************
 ============================================================================*/
/**
 * Parses redirection operators ('<', '>', '2>') from the command arguments.
 * Modifies the `args` array by removing redirection elements and storing them in the `redirections` array.
 *
 * @param args          Pointer to the array of command arguments.
 * @param redirections  Array to store the parsed redirections.
 * @return              The number of redirections parsed, or -1 if an error occurs.
 */
int parse_redirections(char **args, struct redirection *redirections) 
{
	// Return immediately if there are no arguments.
	if (*args == NULL) return -1;

	int count = 0; // Initialize the redirection count.
	char *arg = *args; // Pointer to the first argument.
	// Allocate memory for a new arguments string to store the command without redirection parts.
	char *new_args = malloc(strlen(arg) + 1);
	char *new_args_ptr = new_args; // Pointer to traverse the new_args string.
	int in_quotes = 0; // Flag to track if the current character is inside quotes.

	// Check if memory allocation failed.
	if (new_args == NULL) 
	{
		perror("Memory allocation failed");
		return -1;
	}

	// Iterate through each character in the original arguments.
	while (*arg) 
	{
		// Toggle the in_quotes flag if a quote is encountered.
		if (*arg == '"') 
		{
			in_quotes = !in_quotes;
			*new_args_ptr++ = *arg++; // Copy the quote to new_args and move to the next character.
			continue;
		}

		// If not inside quotes and a redirection operator is found.
		if (!in_quotes && (*arg == '<' || *arg == '>' || (*arg == '2' && *(arg+1) == '>'))) 
		{
			// Check if the maximum number of redirections has been reached.
			if (count >= MAX_REDIRECTIONS) 
			{
				fprintf(stderr, "Too many redirections\n");
				free(new_args); // Free the allocated memory for new_args.
				return -1;
			}

			// Determine the type of redirection (input, output, or error).
			if (*arg == '<') 
			{
				redirections[count].type = 0; // Input redirection.
				arg++;
			} 
			else if (*arg == '>') 
			{
				redirections[count].type = 1; // Output redirection.
				arg++;
			} 
			else 
			{
				redirections[count].type = 2; // Error redirection (2>).
				arg += 2;
			}

			// Skip any spaces following the redirection operator.
			while (*arg == ' ') arg++; 

			char *start = arg; // Mark the start of the file name.
			// If the file name starts with a quote, find the closing quote.
			if (*start == '"') 
			{
				start++;
				arg = strchr(start, '"');
				// If no closing quote is found, report an error.
				if (arg == NULL) 
				{
					fprintf(stderr, "Unmatched quote\n");
					free(new_args); // Free the allocated memory for new_args.
					return -1;
				}
				arg++;  // Move past the closing quote.
			} 
			else 
			{
				// Move the pointer until a space or another redirection operator is found.
				while (*arg && *arg != ' ' && *arg != '<' && *arg != '>' && !(*arg == '2' && *(arg+1) == '>')) arg++;
			}

			// Calculate the length of the file name.
			int len = arg - start;
			// Allocate memory for the file name and store it in the redirections array.
			redirections[count].file = malloc(len + 1);
			if (redirections[count].file == NULL) 
			{
				perror("Memory allocation failed");
				free(new_args); // Free the allocated memory for new_args.
				return -1;
			}
			strncpy(redirections[count].file, start, len);
			redirections[count].file[len] = '\0'; // Null-terminate the file name.

			count++; // Increment the redirection count.
		} 
		else 
		{
			// If not a redirection, copy the character to new_args.
			*new_args_ptr++ = *arg++;
		}
	}
	*new_args_ptr = '\0'; // Null-terminate the new_args string.

	*args = new_args; // Update the args pointer to point to the modified string.
	return count; // Return the number of redirections parsed.
}









/**
 * Applies the redirections parsed from the command arguments by changing the standard input/output/error
 * file descriptors to the corresponding files.
 *
 * @param redirections  Array of redirections to apply.
 * @param count         Number of redirections to apply.
 */
void apply_redirections(struct redirection *redirections, int count) 
{
	// Iterate through each redirection in the array.
	for (int i = 0; i < count; i++) 
	{
		int fd; // File descriptor for the redirection.
		switch (redirections[i].type) 
		{
		case 0:  // Input redirection
			fd = open(redirections[i].file, O_RDONLY); // Open the file for reading.
			if (fd == -1) 
			{
				perror("Failed to open input file");
				exit(EXIT_FAILURE); // Exit if the file cannot be opened.
			}
			if (dup2(fd, STDIN_FILENO) == -1) 
			{
				perror("Failed to redirect input");
				exit(EXIT_FAILURE); // Exit if the input cannot be redirected.
			}
			close(fd); // Close the file descriptor.
			break;

		case 1:  // Output redirection
			// Open the file for writing (truncate or create if it doesn't exist).
			fd = open(redirections[i].file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1) 
			{
				perror("Failed to open output file");
				exit(EXIT_FAILURE); // Exit if the file cannot be opened.
			}
			if (dup2(fd, STDOUT_FILENO) == -1) 
			{
				perror("Failed to redirect output");
				exit(EXIT_FAILURE); // Exit if the output cannot be redirected.
			}
			close(fd); // Close the file descriptor.
			break;

		case 2:  // Error redirection
			// Open the file for writing (truncate or create if it doesn't exist).
			fd = open(redirections[i].file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1) 
			{
				perror("Failed to open error file");
				exit(EXIT_FAILURE); // Exit if the file cannot be opened.
			}
			if (dup2(fd, STDERR_FILENO) == -1) 
			{
				perror("Failed to redirect error");
				exit(EXIT_FAILURE); // Exit if the error output cannot be redirected.
			}
			close(fd); // Close the file descriptor.
			break;
		}
	}
}







/**
 * Trim leading and trailing whitespace from a string.
 *
 * This function modifies the string in place, meaning the original string passed to it will be altered directly.
 *
 * Leading whitespace characters are removed by advancing the string pointer to the first non-whitespace character. 
 *
 * Trailing whitespace characters are removed by setting a null terminator after the last non-whitespace character. 
 *
 * If the input string is NULL or consists entirely of whitespace, the function will return without making any changes.
 *
 * @param str: Pointer to the input string (char*). This is the string that will be trimmed
 *             of leading and trailing whitespace characters. The string must be null-terminated.
 */
void Remove_Leading_Trailing_Whitespaces(char *str) 
{
	// If the input string is NULL, there is nothing to process, so return immediately.
	if (str == NULL) return;

	// Declares a pointer end which will be used to find the end of the string. 
	char *end;

	// Move the 'str' pointer forward to skip all leading whitespace characters.
	// The 'isspace' function checks if the character is a whitespace character and return True if it is whitespace.
	while(isspace((unsigned char)*str)) str++;

	// If the resulting string is empty (all whitespace), return immediately.
	if(*str == 0) return;

	// Set 'end' to point to the last character of the string (before the null terminator).
	end = str + strlen(str) - 1;

	// Move the 'end' pointer backward to skip all trailing whitespace characters.
	while(end > str && isspace((unsigned char)*end)) end--;

	// Place a null terminator after the last non-whitespace character to end the string.
	// This effectively removes the trailing whitespace.
	end[1] = '\0';
}










/**
 * Parses a command pipeline from the input string, separating commands by the pipe (`|`) operator.
 *
 * @param input    The input string containing the command pipeline.
 * @param commands An array of `struct command` to store the parsed commands.
 * @return         The number of commands in the pipeline.
 */
int parse_pipeline(char *input, struct command *commands) 
{
	char *cmd_str;  // Pointer to the current command string.
	int cmd_count = 0;  // Counter for the number of commands in the pipeline.

	// Loop through the input string, splitting it at each pipe character.
	while ((cmd_str = strsep(&input, "|")) != NULL && cmd_count < MAX_PIPES) 
	{
		char *token;
		commands[cmd_count].argc = 0;  // Initialize the argument count for the current command.

		// Remove leading and trailing whitespaces from the command string.
		Remove_Leading_Trailing_Whitespaces(cmd_str);

		// Parse any redirections in the current command string.
		commands[cmd_count].redirection_count = parse_redirections(&cmd_str, commands[cmd_count].redirections);

		int in_quotes = 0;  // Flag to track whether we are inside a quoted string.
		char *start = cmd_str;  // Pointer to the start of the current argument.

		// Loop through the command string to extract individual arguments.
		for (char *p = cmd_str; ; p++) 
		{
			if (*p == '"') 
			{
				in_quotes = !in_quotes;  // Toggle the in_quotes flag when encountering a quote.
			} 
			else if ((*p == ' ' && !in_quotes) || *p == '\0') 
			{
				if (start != p) 
				{
					// Allocate memory for the argument and copy it from the command string.
					int len = p - start;
					char *arg = malloc(len + 1);
					strncpy(arg, start, len);
					arg[len] = '\0';
					commands[cmd_count].argv[commands[cmd_count].argc++] = arg;  // Store the argument in the argv array.
				}
				start = p + 1;  // Move the start pointer to the next character after the space.
				if (*p == '\0') break;  // If the end of the string is reached, exit the loop.
			}
		}

		// Null-terminate the argv array for the current command.
		commands[cmd_count].argv[commands[cmd_count].argc] = NULL;

		cmd_count++;  // Increment the command count.
	}

	return cmd_count;  // Return the total number of commands in the pipeline.
}








/**
 * Executes a pipeline of commands, connecting them with pipes as needed.
 *
 * @param commands An array of `struct command` containing the parsed commands.
 * @param cmd_count The number of commands in the pipeline.
 */
void execute_pipeline(struct command *commands, int cmd_count) 
{
	int pipes[MAX_PIPES][2];  // Array to store file descriptors for the pipes.
	pid_t pids[MAX_PIPES + 1];  // Array to store process IDs for each command.

	// Create pipes between consecutive commands.
	for (int i = 0; i < cmd_count - 1; i++) 
	{
		if (pipe(pipes[i]) == -1) 
		{
			perror("pipe");  // Print an error message if pipe creation fails.
			exit(EXIT_FAILURE);  // Exit with failure status.
		}
	}

	// Loop through each command in the pipeline.
	for (int i = 0; i < cmd_count; i++) 
	{
		pids[i] = fork();  // Fork a new process for the current command.

		if (pids[i] == -1) 
		{
			perror("fork");  // Print an error message if fork fails.
			exit(EXIT_FAILURE);  // Exit with failure status.
		} 
		else if (pids[i] == 0) 
		{
			// Child process
			if (i > 0) 
			{
				dup2(pipes[i-1][0], STDIN_FILENO);  // Redirect the input to the read end of the previous pipe.
			}
			if (i < cmd_count - 1) 
			{
				dup2(pipes[i][1], STDOUT_FILENO);  // Redirect the output to the write end of the current pipe.
			}

			// Close all pipe file descriptors in the child process.
			for (int j = 0; j < cmd_count - 1; j++) 
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			// Apply any redirections specified for the current command.
			apply_redirections(commands[i].redirections, commands[i].redirection_count);

			// Execute the command.
			execvp(commands[i].argv[0], commands[i].argv);
			perror("execvp");  // Print an error message if execvp fails.
			exit(EXIT_FAILURE);  // Exit with failure status.
		}
	}

	// Close all pipe file descriptors in the parent process.
	for (int i = 0; i < cmd_count - 1; i++) 
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
	}

	// Wait for all child processes to complete.
	for (int i = 0; i < cmd_count; i++) 
	{
		int status;
		waitpid(pids[i], &status, 0);  // Wait for the child process with PID pids[i].
	}
}








/**
 * Executes a single external command with arguments, handling redirections and recording the process in history.
 *
 * @param cmd   The command to execute.
 * @param args  The arguments for the command, as a string.
 * @param input The full input string for history tracking.
 */
void Execute_External_Command(char *cmd, char *args, char *input) 
{
	pid_t pid;  // Process ID for the child process.
	int status;  // Variable to store the exit status of the child process.
	struct command command;  // Structure to store the command and its arguments.

	command.argc = 0;  // Initialize the argument count.
	command.argv[command.argc++] = cmd;  // Store the command as the first argument.

	if (args != NULL) 
	{
		// Duplicate the arguments string to parse it.
		char *parsed_args = strdup(args);
		command.redirection_count = parse_redirections(&parsed_args, command.redirections);  // Parse redirections.

		// Tokenize the arguments string and store each argument in argv.
		char *token = strtok(parsed_args, " ");
		while (token != NULL && command.argc < MAX_NUMBER_OF_ARGUMENTS - 1) 
		{
			command.argv[command.argc++] = token;
			token = strtok(NULL, " ");
		}
	}
	command.argv[command.argc] = NULL;  // Null-terminate the argv array.

	pid = fork();  // Fork a new process for the command.
	if (pid == FORK_FAILED) 
	{
		perror("Fork failed");  // Print an error message if fork fails.
		return;
	} 
	else if (pid == IS_CHILD) 
	{
		// Child process
		apply_redirections(command.redirections, command.redirection_count);  // Apply redirections.
		execvp(cmd, command.argv);  // Execute the command.
		fprintf(stderr, "execvp error for %s: %s\n", cmd, strerror(errno));  // Print an error message if execvp fails.
		exit(EXIT_FAILURE);  // Exit with failure status.
	} 
	else 
	{
		// Parent process
		if (waitpid(pid, &status, 0) == PROCESS_FAILED) 
		{
			perror("waitpid failed");  // Print an error message if waitpid fails.
			return;
		}

		// Handle the child process's exit status.
		if (WIFEXITED(status)) 
		{
			add_to_process_history(input, WEXITSTATUS(status));  // Record the command and its exit status in history.
		} 
		else if (WIFSIGNALED(status)) 
		{
			fprintf(stderr, "Child process terminated by signal %d\n", WTERMSIG(status));  // Handle termination by signal.
			add_to_process_history(input, -WTERMSIG(status));  // Record the command and its signal termination in history.
		}
	}
}









/**
 * Execute a single command.
 *
 * This function determines whether the provided command is an internal command or an external command.
 * If it is an internal command, it executes the associated function. Otherwise, it attempts to execute 
 * the command as an external command by calling the appropriate function.
 *
 * @param cmd: A struct representing the command to be executed, which includes the command name 
 *             and its arguments.
 * @param input: A pointer to the input string that contains the entire command line input.
 */
void execute_single_command(struct command cmd, char *input)
{
    // Initialize a boolean variable to track if the command is an internal command.
    bool command_found = false;

    // Loop through the list of internal commands to check if the given command matches any internal command.
    for (int i = 0; internal_commands[i].name != NULL; i++) 
    {
        // Compare the command name with the current internal command's name.
        if (strcmp(cmd.argv[0], internal_commands[i].name) == STRINGS_ARE_EQUAL) 
        {
            // Skip the command name in the input string to isolate the arguments.
            char *args = input + strlen(cmd.argv[0]);
            
            // Skip any leading spaces in the arguments.
            while (*args == ' ') args++;
            
            // Call the function associated with the internal command, passing the arguments.
            internal_commands[i].function(args);
            
            // Mark that the command was found as an internal command.
            command_found = true;
            
            // Exit the loop as the command has been found and executed.
            break;
        }
    }

    // If the command was not found in the list of internal commands, handle it as an external command.
    if (!command_found) 
    {
        // Isolate the command name from the struct.
        char *command = cmd.argv[0];
        
        // Skip the command name in the input string to isolate the arguments.
        char *args = input + strlen(command);
        
        // Skip any leading spaces in the arguments.
        while (*args == ' ') args++;
        
        // Execute the command as an external command using the appropriate function.
        Execute_External_Command(command, args, input);
    }
}







/**
 * Execute a command or a pipeline of commands.
 *
 * This function takes an input command string, parses it to determine whether it contains
 * a pipeline of commands, and then either executes a single command or a pipeline of commands
 * based on the parsed result.
 *
 * @param input: A pointer to the input string containing the command(s) to be executed. 
 *               This string may contain a single command or multiple commands separated by pipes.
 */
void execute_command(char *input)
{
    // Declare an array to hold the parsed commands, with a maximum size defined by MAX_PIPES.
    struct command commands[MAX_PIPES];
    
    // Parse the input string to identify and split commands in a pipeline. 
    // The function returns the number of commands parsed.
    int cmd_count = parse_pipeline(input, commands);

    // If there is no pipeline (only a single command), execute the single command.
    if (cmd_count == NO_PIPELINE) 
    {
        execute_single_command(commands[0], input);
    } 
    // If there are multiple commands in the pipeline, execute the pipeline.
    else if (cmd_count > NO_PIPELINE) 
    {
        execute_pipeline(commands, cmd_count);
    }
}






/**
 * Process the input command string.
 *
 * This function handles different types of input commands, including variable assignments,
 * special commands like "allVar" and "myexit," and general command execution. It returns 
 * a boolean indicating whether the shell should continue running.
 *
 * @param input: A pointer to the input string containing the command to be processed. 
 *               This string may represent a variable assignment, a special command, 
 *               or a general command.
 * 
 * @return: A boolean value. Returns `false` to indicate that the shell should continue running.
 *          If the command is "myexit," it returns the result of `cmd_exit()`, which typically
 *          signals whether to exit the shell.
 */
bool process_input(char *input)
{
    // Check if the input is a variable assignment and handle it if true.
    if (is_variable_assignment(input)) 
    {
        handle_variable_assignment(input);
        return false;
    } 
    // Check if the input is the "myexit" command, which may signal the shell to exit.
    else if (strcmp(input, "myexit") == STRINGS_ARE_EQUAL) 
    {
        return cmd_exit();
    } 
    // If the input is none of the above, treat it as a general command and execute it.
    else 
    {
        execute_command(input);
        return false;
    }
}



/*============================================================================
 ******************************  Main Code  **********************************
 ============================================================================*/

/**
 * Main function for the command-line interpreter.
 *
 * This function initializes the command-line interface, processes user input in a loop, and handles 
 * the exit condition for the shell. It reads user input, processes it, and decides whether the shell 
 * should continue running based on the commands provided.
 *
 * @return: An integer value representing the exit status of the program. Typically, `0` indicates 
 *          a successful execution.
 */
int main() 
{
    // Pointer to hold the user input string.
    char *input;
    
    // Boolean flag to determine whether the shell should exit the main loop.
    bool should_exit = false;

    // Bind the tab key to the default readline insert function, allowing for regular tab usage.
    rl_bind_key('\t', rl_insert);

    // Main loop of the shell that continues until the user requests to exit.
    while (!should_exit) 
    {
        // Display the prompt and read a line of input from the user.
        input = readline(PROMPT);

        // If the input is NULL (e.g., if EOF is encountered), print a newline and break the loop.
        if (input == NULL) 
        {
            printf("\n");
            break;
        }

        // Remove leading and trailing whitespace from the input string.
        Remove_Leading_Trailing_Whitespaces(input);

        // If the input string is empty after trimming, free the memory and continue to the next iteration.
        if (strlen(input) == 0) 
        {
            free(input);
            continue;
        }

        // Add the non-empty input string to the command history.
        add_history(input);
        
        // Process the input and check if the shell should exit based on the command.
        should_exit = process_input(input);

        // Free the memory allocated for the input string.
        free(input);
    }

    // Clear the command history before exiting the program.
    rl_clear_history();
    
    // Return 0 to indicate successful execution of the program.
    return 0;
}
