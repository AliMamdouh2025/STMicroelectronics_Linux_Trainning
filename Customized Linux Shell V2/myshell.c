/**
 *===================================================================================
 * @file           : commands.c
 * @author         : Ali Mamdouh
 * @brief          : Impelementation of commands to be executed bu customized shell
 * @Reviwer        : Eng Kareem
 * @Version        : 2.0.0
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
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "commands.h"
#include <stdbool.h>
#include <sys/wait.h>
#include <errno.h>



/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/
#define MAX_INPUT                               1024
#define PROMPT                                  "AliMamdouhShell > "
#define MAX_NUMBER_OF_ARGUMENTS                 64




/*============================================================================
 ********************************  Macros  ***********************************
 ============================================================================*/
#define INCREMENT_POINTER_BY_1                  1
#define STRINGS_ARE_EQUAL                       0
#define FORK_FAILED                            -1
#define WAIT_PID_FAILED                        -1
#define IS_CHILD                                0





/*============================================================================
 **************************  Functions Definitions  **************************
 ============================================================================*/

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
 * Execute an external command with arguments.
 *
 * This function creates a child process to execute the specified command with the given arguments.
 * The parent process waits for the child process to complete and then records the command and its exit status.
 *
 * The function dynamically allocates memory for the argument vector (argv) and uses fork() to create a new process.
 * The child process replaces its image with a new image of the command using execvp(), and the parent process waits
 * for the child to complete using waitpid().
 *
 * If memory allocation or reallocation fails, or if fork() fails, an error message is printed.
 * If the child process fails to execute the command, an error message is printed and the child process exits.
 *
 * @param cmd: Pointer to the command string (char*). This is the command to be executed.
 * @param args: Pointer to the arguments string (char*). This string contains the arguments for the command, 
 *              separated by spaces.
 * @param input: Pointer to the input string (char*). This string is used to record the command and its arguments
 *               along with the exit status in the process history.
 */
void Execute_External_Command(char *cmd, char *args, char *input)
{	
	pid_t pid;
	char **argv = NULL;
	int argc = 0;
	char *token;
	int status;

	// Allocate initial memory for argv
	argv = malloc(sizeof(char *));
	if (argv == NULL) 
	{
		perror("Memory allocation failed");
		return;
	}

	// Add the command as the first argument
	argv[argc++] = cmd;

	// Tokenize the arguments string and populate the argv array
	token = strtok(args, " ");
	while (token != NULL) 
	{
		char **temp = realloc(argv, (argc + 1) * sizeof(char *));
		if (temp == NULL) 
		{
			perror("Memory reallocation failed");
			free(argv);
			return;
		}
		argv = temp;
		argv[argc++] = token;
		token = strtok(NULL, " ");
	}

	// Null-terminate the argv array
	char **temp = realloc(argv, (argc + 1) * sizeof(char *));
	if (temp == NULL) 
	{
		perror("Memory reallocation failed");
		free(argv);
		return;       
	}
	argv = temp;
	argv[argc] = NULL;

	// Fork a child process
	pid = fork();
	if (pid == FORK_FAILED) 
	{
		perror("Fork failed");
		free(argv);
		return;

	} else if (pid == IS_CHILD) 
	{
		// Child process
		execvp(cmd, argv);

		// If execvp returns, it must have failed
		fprintf(stderr, "execvp error for %s: %s\n", cmd, strerror(errno));
		free(argv);
		exit(EXIT_FAILURE);
	} else 
	{
		// Parent process
		//Setting waitpid(, , 0) means that waitpid behaves in a blocking manner-
		// meaning it will wait until the specified child process terminates.
		if (waitpid(pid, &status, 0) == WAIT_PID_FAILED) 
		{
			perror("waitpid failed");
			free(argv);
			return;
		}

                 // Checks if the child process terminated normally-
                // (i.e., it called exit() or returned from main()).
		if (WIFEXITED(status)) 
		{
			 // WEXITSTATUS(status): Extracts the exit status code of the child process. 
			// This is the value passed to exit() by the child process.
			add_to_process_history(input, WEXITSTATUS(status));

                   // Checks if the child process was terminated by a signal-
                  // (e.g., SIGKILL or SIGSEGV).
		} else if (WIFSIGNALED(status)) 
		{
			 // WTERMSIG(status): Extracts the signal number that caused the child process to terminate.
                        // This provides information about why the child process was killed.
			fprintf(stderr, "Child process terminated by signal %d\n", WTERMSIG(status));
			
	          	 // Records the command and the signal number that terminated the child process in the process history. 
			// The negative sign (-WTERMSIG(status)) indicates that the process was killed by a signal, rather than exiting normally.
			add_to_process_history(input, -WTERMSIG(status));
		}
	}
	// Free the allocated memory for argv
	free(argv);
}





/*============================================================================
 ******************************  Main Code  **********************************
 ============================================================================*/

/* This code to take the arguments and command but not from the 'main()' arguments but from strtok() */
int main() 
{
	// Declares a pointer to a character array (string) that will hold the user's input.
	char *input;

	// Declares two pointers to character arrays that will hold the command and its arguments, respectively.
	char *cmd, *args;

	// A boolean flag to control the main loop of the shell. Initially set to false, indicating that the shell should continue running.
	bool should_exit = false;

	// This function tells the Readline library to bind the tab key to the rl_insert function.
	// When the tab key is pressed, instead of performing its default action (like auto-completion), the Readline library will call the rl_insert function.
	// The rl_insert function's role is to simply insert the tab character (\t) into the input buffer at the current cursor position.
	rl_bind_key('\t', rl_insert);

	// Starts a loop that continues until should_exit is set to true.
	while (!should_exit) 
	{
		// Calls the readline function from the readline library to display the prompt (PROMPT) and read a line of input from the user. 
		input = readline(PROMPT);

		// Checks if the input is NULL, which indicates that EOF (End of File) was entered, typically by pressing Ctrl+D.
		if (input == NULL)
		{  
			printf("\n");
			break;
		}

		// Calls the RemoveWhitespaces function to trim leading and trailing whitespace from the input string.   
		Remove_Leading_Trailing_Whitespaces(input);



		// Checks if the length of the input string is 0 after trimming. 
		// If the input is empty (i.e., it contained only whitespace), it proceeds to free the allocated memory and igmore this iteration.
		if (strlen(input) == 0) 
		{
			free(input);
			continue;
		}

		// Adds the input string to the readline history, allowing the user to recall it later using the up and down arrow keys.
		add_history(input);

		// strtok will find the first token in the input string, which is the substring before the first space.
		cmd = strtok(input, " ");

		// Continues tokenizing the string as we pass NULL in the first arguments
		// Delimiter: "\n" means it will take everything until it finds "\n"
		args = strtok(NULL, "\n");

		//Filter Quters of Argument if they exists
		args = extract_quoted_arg(args);



		// Checks if cmd is NULL, which could happen if the input string was empty or contained only spaces, Then ignore this iteration.
		if (cmd == NULL) 
		{
			free(input);
			continue;
		}


		/*Select Command*/
		if (strcmp(cmd, "mycp") == STRINGS_ARE_EQUAL) 
		{
			cmd_mycp(args);

		} else if (strcmp(cmd, "mymv") == STRINGS_ARE_EQUAL) 
		{
			cmd_mymv(args);

		} else if (strcmp(cmd, "mypwd") == STRINGS_ARE_EQUAL) 
		{
			cmd_pwd();

		} else if (strcmp(cmd, "myecho") == STRINGS_ARE_EQUAL) 
		{
			cmd_echo(args ? args : "");

		} else if (strcmp(cmd, "myhelp") == STRINGS_ARE_EQUAL) 
		{
			cmd_help();

		} else if (strcmp(cmd, "myexit") == STRINGS_ARE_EQUAL) 
		{
			should_exit = cmd_exit();

		} else if (strcmp(cmd, "mycd") == STRINGS_ARE_EQUAL) 
		{
			cmd_cd(args);

		} else if (strcmp(cmd, "mytype") == STRINGS_ARE_EQUAL) 
		{
			cmd_type(args);

		} else if (strcmp(cmd, "myenvir") == STRINGS_ARE_EQUAL) 
		{
			cmd_envir(args);

		} else if (strcmp(cmd, "myphist") == STRINGS_ARE_EQUAL) 
		{
			cmd_phist();

		} else 
		{
			Execute_External_Command(cmd, args, input);
		}

		free(input);
	}

	// Clears the history of commands stored by the readline library
	rl_clear_history();

	return 0;
}
