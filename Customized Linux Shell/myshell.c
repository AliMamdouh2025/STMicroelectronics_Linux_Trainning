/**
 *==================================================================================
 * @file           : myshell.c
 * @author         : Ali Mamdouh
 * @brief          : Simple shell of linux that can takes commands from user
 *==================================================================================
 * 
 *==================================================================================
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



/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/
#define MAX_INPUT                               1024
#define PROMPT                                  "AliMamdouhShell > "




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
void RemoveWhitespaces(char *str) 
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



/*============================================================================
 ******************************  Main Code  **********************************
 ============================================================================*/

int main() // This code to take the arguments and command but not from the 'main()' arguments but from strtok() 
{
	// Declares a pointer to a character array (string) that will hold the user's input.
	char *input;

	// Declares two pointers to character arrays that will hold the command and its arguments, respectively.
	char *cmd, *args;

	// A boolean flag to control the main loop of the shell. Initially set to false, indicating that the shell should continue running.
	bool should_exit = false;

	// Initialize readline
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
		RemoveWhitespaces(input);

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

		// Checks if cmd is NULL, which could happen if the input string was empty or contained only spaces, Then ignore this iteration.
		if (cmd == NULL) 
		{
			free(input);
			continue;
		}


		/*Select Command*/
		if (strcmp(cmd, "pwd") == 0) 
		{
			cmd_pwd();
		} else if (strcmp(cmd, "echo") == 0)
		{
			cmd_echo(args ? args : ""); // calls the cmd_echo function with args as an argument. If args is NULL, it passes an empty string.
		} else if (strcmp(cmd, "cp") == 0) 
		{
			cmd_cp(args); // calls the cmd_cp function with args as an argument.
		} else if (strcmp(cmd, "mv") == 0) 
		{
			cmd_mv(args); // calls the cmd_mv function with args as an argument.
		} else if (strcmp(cmd, "help") == 0) 
		{
			cmd_help();
		} else if (strcmp(cmd, "exit") == 0) 
		{
			should_exit = cmd_exit();
		} else 
		{
			fprintf(stderr, "Unknown command: %s\n", cmd);
		}

		free(input);
	}

	// Clears the history of commands stored by the readline library
	rl_clear_history();

	return 0;
}
