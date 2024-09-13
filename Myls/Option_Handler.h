 /*===================================================================================
 * @file           : Option_Handler.h
 * @author         : Ali Mamdouh
 * @brief          : header of Option_Handler
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
 *===================================================================================
 * 
 *===================================================================================
 */


#ifndef _OPTIONS_H_
#define _OPTIONS_H_
#define _OPTIONS_H_


/*============================================================================
 **********************************  Macros  *********************************
 ============================================================================*/ 
/* Option Flags */

/**
 * @brief Option flag for long format output.
 * 
 * This flag indicates that the long format option (`-l`) is enabled.
 */
#define LONG_FORMAT_OPTION_l                0

/**
 * @brief Option flag for showing hidden files.
 * 
 * This flag indicates that the option to show hidden files (`-a`) is enabled.
 */
#define SHOW_HIDDEN_OPTION_a                1

/**
 * @brief Option flag for sorting by modification time.
 * 
 * This flag indicates that the option to sort files by modification time (`-t`) is enabled.
 */
#define SORT_BY_TIME_OPTION_t               2

/**
 * @brief Option flag for sorting by access time.
 * 
 * This flag indicates that the option to sort files by access time (`-u`) is enabled.
 */
#define ACCESS_TIME_OPTION_u                3

/**
 * @brief Option flag for sorting by change time.
 * 
 * This flag indicates that the option to sort files by change time (`-c`) is enabled.
 */
#define CHANGE_TIME_OPTION_c                4

/**
 * @brief Option flag for showing inode numbers.
 * 
 * This flag indicates that the option to show inode numbers (`-i`) is enabled.
 */
#define SHOW_INODE_OPTION_i                 5

/**
 * @brief Option flag for disabling all other options.
 * 
 * This flag indicates that the option to disable all other options (`-f`) is enabled.
 */
#define DISABLE_EVERYTING_OPTION_f          6

/**
 * @brief Option flag for showing the directory itself.
 * 
 * This flag indicates that the option to show the directory itself (`-d`) is enabled.
 */
#define SHOW_DIRECTORY_ITSELF_OPTION_d      7

/**
 * @brief Option flag for showing one file per line.
 * 
 * This flag indicates that the option to show one file per line (`-1`) is enabled.
 */
#define SHOW_1_FILE_IN_LINE_OPTION_1        8

/* Constants */

/**
 * @brief Maximum length of file paths.
 * 
 * This constant defines the maximum length for file paths, set to 2048 characters.
 */
#define MAX_PATH_LENGTH                     2048    

/**
 * @brief Maximum number of files.
 * 
 * This constant defines the maximum number of files that can be handled, set to 1000.
 */
#define MAX_FILES                           1000          

/* File Mode Constants */

/**
 * @brief Define the `S_ISVTX` macro if it is not already defined.
 * 
 * The `S_ISVTX` macro is used to test the sticky bit on files, indicating that only the file owner can delete or rename the file.
 */
#ifndef S_ISVTX
#define S_ISVTX                             01000
#endif






/*============================================================================
 ********************************  Functions  ********************************
 ============================================================================*/ 
void Print_ls_WithoutLongFormat( char *file_names[],int file_count, const char *dir );
void Print_ls_LongFormat(char *file_names[], int file_count, const char *dir);
void Execute_ls(char *dir);

#endif