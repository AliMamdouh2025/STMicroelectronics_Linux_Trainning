/**
 *===================================================================================
 * @file           : variables.h
 * @author         : Ali Mamdouh
 * @brief          : Header of variables.c
 * @Reviwer        : Eng Kareem
 * @Version        : 3.1.0
 * @Company        : STMicroelectronics
 *===================================================================================
 * 
 *===================================================================================
 */

#ifndef VARIABLES_H
#define VARIABLES_H



/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/
#include <stdbool.h>



/*============================================================================
 **************************  Functions Declerations  *************************
 ============================================================================*/
// Function to set a local variable
void set_local_variable(const char *name, const char *value);

// Function to get a local variable
const char *get_local_variable(const char *name);

// Function to print all variables (local and environment)
void print_all_variables(void);

// Function to check if a string is a variable assignment
bool is_variable_assignment(const char *input);

// Function to handle variable assignment
void handle_variable_assignment(const char *input);

#endif // VARIABLES_H
