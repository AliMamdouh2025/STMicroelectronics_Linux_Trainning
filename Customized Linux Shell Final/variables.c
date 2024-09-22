/**
 *===================================================================================
 * @file           : variables.c
 * @author         : Ali Mamdouh
 * @brief          : defines functions to manage and print local variables, 
 *                   check for variable assignments, and handle variable assignment operations.
 * @Reviwer        : Eng Kareem
 * @Version        : 3.1.0
 * @Company        : STMicroelectronics
 *===================================================================================
 * 
 *===================================================================================
 */



/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/
#include "variables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/
#define MAX_LOCAL_VARIABLES                           100
#define MAX_VARIABLE_NAME_LENGTH                      50
#define MAX_VARIABLE_VALUE_LENGTH                     1000





/*============================================================================
 *************************  Data types Declerations  *************************
 ============================================================================*/
struct LocalVariable 
{
	char name[MAX_VARIABLE_NAME_LENGTH];
	char value[MAX_VARIABLE_VALUE_LENGTH];
};



/*============================================================================
 **********************  Globaal Variables Definition  ***********************
 ============================================================================*/
static struct LocalVariable local_variables[MAX_LOCAL_VARIABLES] = {0};
static int local_variable_count = 0;




/*============================================================================
 **************************  Functions Definitions  **************************
 ============================================================================*/
/**
 * Sets the value of a local variable. If the variable already exists, its value is updated.
 * If the variable does not exist and the maximum number of local variables has not been reached,
 * a new variable is created.
 *
 * @param name  The name of the variable to set.
 * @param value The value to assign to the variable.
 */
void set_local_variable(const char *name, const char *value) 
{
	// Iterate through the list of existing local variables.
	for (int i = 0; i < local_variable_count; i++) 
	{
		// Check if the current variable's name matches the given name.
		if (strcmp(local_variables[i].name, name) == 0) 
		{
			// Copy the new value into the variable's value field.
			strncpy(local_variables[i].value, value, MAX_VARIABLE_VALUE_LENGTH - 1);
			// Ensure the value is null-terminated.
			local_variables[i].value[MAX_VARIABLE_VALUE_LENGTH - 1] = '\0';
			return; // Exit the function since the variable has been updated.
		}
	}

	// If the variable was not found, check if there is space to add a new one.
	if (local_variable_count < MAX_LOCAL_VARIABLES) 
	{
		// Copy the variable's name into the new entry.
		strncpy(local_variables[local_variable_count].name, name, MAX_VARIABLE_NAME_LENGTH - 1);
		// Ensure the name is null-terminated.
		local_variables[local_variable_count].name[MAX_VARIABLE_NAME_LENGTH - 1] = '\0';
		// Copy the value into the new entry.
		strncpy(local_variables[local_variable_count].value, value, MAX_VARIABLE_VALUE_LENGTH - 1);
		// Ensure the value is null-terminated.
		local_variables[local_variable_count].value[MAX_VARIABLE_VALUE_LENGTH - 1] = '\0';
		// Increment the count of local variables.
		local_variable_count++;
	} 
	else 
	{
		// If the maximum number of local variables has been reached, print an error message.
		fprintf(stderr, "Error: Maximum number of local variables reached\n");
	}
}







/**
 * Retrieves the value of a local variable given its name.
 *
 * @param name The name of the variable to retrieve.
 * @return     The value of the variable, or NULL if the variable does not exist.
 */
const char *get_local_variable(const char *name) 
{
	// Iterate through the list of local variables.
	for (int i = 0; i < local_variable_count; i++) 
	{
		// Check if the current variable's name matches the given name.
		if (strcmp(local_variables[i].name, name) == 0) 
		{
			// Return the value associated with the variable name.
			return local_variables[i].value;
		}
	}
	// Return NULL if the variable was not found.
	return NULL;
}







/**
 * Prints all local variables and environment variables.
 * Local variables are printed in the format: name=value.
 * Environment variables are printed as they are stored.
 */
void print_all_variables(void) 
{
	// Print a header for the local variables.
	printf("Local Variables:\n");
	// Iterate through the list of local variables.
	for (int i = 0; i < local_variable_count; i++) 
	{
		// Print the name and value of each local variable.
		printf("%s=%s\n", local_variables[i].name, local_variables[i].value);
	}

	// Print a newline and a header for the environment variables.
	printf("\nEnvironment Variables:\n");
	// Access the list of environment variables.
	extern char **environ;
	// Iterate through the environment variables.
	for (char **env = environ; *env != NULL; env++) 
	{
		// Print each environment variable.
		printf("%s\n", *env);
	}
}






/**
 * Checks if a given input string represents a variable assignment.
 *
 * @param input The input string to check.
 * @return      True if the input contains an equals sign (=) and the sign is not at the start of the string.
 */
bool is_variable_assignment(const char *input) 
{
	// Find the first occurrence of the equals sign in the input string.
	char *equals = strchr(input, '=');
	// Return true if the equals sign is present and not at the start of the string.
	return equals != NULL && equals != input;
}







/**
 * Handles a variable assignment by parsing the input and setting the corresponding local variable.
 *
 * @param input The input string containing the variable assignment in the format: name=value.
 */
void handle_variable_assignment(const char *input) 
{
	// Find the first occurrence of the equals sign in the input string.
	char *equals = strchr(input, '=');
	// If there is no equals sign, return early.
	if (equals == NULL) return;

	// Calculate the length of the variable name (characters before the equals sign).
	int name_length = equals - input;
	// Declare a buffer to hold the variable name.
	char name[MAX_VARIABLE_NAME_LENGTH];
	// Copy the variable name into the buffer.
	strncpy(name, input, name_length);
	// Ensure the variable name is null-terminated.
	name[name_length] = '\0';

	// Get the value part of the assignment (characters after the equals sign).
	const char *value = equals + 1;
	// Set the local variable with the parsed name and value.
	set_local_variable(name, value);
}
