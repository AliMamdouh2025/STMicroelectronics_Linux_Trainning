/**
 *===================================================================================
 * @file           : commands.h
 * @author         : Ali Mamdouh
 * @brief          : Header of command.c
 * @Reviwer        : Eng Kareem
 * @Version        : 2.0.0
 * @Company        : STMicroelectronics
 * @Date           : 30/7/2024 
 *===================================================================================
 * 
 *===================================================================================
 */
#ifndef COMMANDS_H
#define COMMANDS_H



/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/
#include <stdbool.h>



/*============================================================================
 *****************************  Config Macros  *******************************
 ============================================================================*/
#define MAX_PATH                     4096
#define BUFFER_SIZE                  4096
#define MAX_PROCESS_HISTORY          10
#define MAX_COMMAND_LENGTH           256



/*============================================================================
 *************************  Data types Declerations  *************************
 ============================================================================*/
struct ProcessInfo 
{
	char command[MAX_COMMAND_LENGTH];
	int exit_status;
}; 




/*============================================================================
 **************************  Variables Decleration  **************************
 ============================================================================*/
extern struct ProcessInfo process_history[MAX_PROCESS_HISTORY];
extern int process_history_count;



/*============================================================================
 ***********************  Functions Helper Definitions  **********************
 ============================================================================*/
char* extract_quoted_arg(char* input);
void tokenize_Paths(const char *MainArgument, char **FirstPath, char **SecondPath); 




/*============================================================================
 ***********************  Functions Command Definitions  *********************
 ============================================================================*/
void cmd_pwd(void);
void cmd_echo(char *args);
void cmd_mycp(char *args);
void cmd_mymv(char *args);
void cmd_help(void);
bool cmd_exit(void);
void cmd_cd(char *path);
void cmd_type(char *command);
void cmd_envir(char *variable);
void cmd_phist(void);
void add_to_process_history(const char *command, int exit_status);

#endif
