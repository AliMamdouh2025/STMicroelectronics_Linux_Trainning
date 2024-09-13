 /*===================================================================================
 * @file           : Myls.c
 * @author         : Ali Mamdouh
 * @brief          : Simulate ls command
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
 *===================================================================================
 * 
 *===================================================================================
 */




/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "Option_Handler.h"




/*============================================================================
 **********************  Global Variables Decleration  ***********************
 ============================================================================*/ 
extern char *optarg;
extern int optind, opterr, optopt;

/* Array to carry the state of options */
extern int OptionsFlags[9];






/*============================================================================
 *****************************  Functions  ***********************************
 ============================================================================*/ 

/**
 * Lists the contents of a directory. This Function is made for the purpose of abstraction only.
 *
 * @param dir: Directory name.
 */
static void list_directory(char *dir)
{
    Execute_ls(dir);  // Function to list the directory contents
}




/**
 * Sets the appropriate option flag based on the parsed command-line option.
 *
 * @param opt: Parsed option character.
 */
static void set_option_flag(int opt)
{
    switch (opt) 
    {
        case 'l': OptionsFlags[LONG_FORMAT_OPTION_l] = 1;               break;
        case 'a': OptionsFlags[SHOW_HIDDEN_OPTION_a] = 1;               break;
        case 't': OptionsFlags[SORT_BY_TIME_OPTION_t] = 1;              break;
        case 'u': OptionsFlags[ACCESS_TIME_OPTION_u] = 1;               break;
        case 'c': OptionsFlags[CHANGE_TIME_OPTION_c] = 1;               break;
        case 'i': OptionsFlags[SHOW_INODE_OPTION_i] = 1;                break;
        case 'f': OptionsFlags[DISABLE_EVERYTING_OPTION_f] = 1;         break;
        case 'd': OptionsFlags[SHOW_DIRECTORY_ITSELF_OPTION_d] = 1;     break;
        case '1': OptionsFlags[SHOW_1_FILE_IN_LINE_OPTION_1] = 1;       break;
        default:  
            fprintf(stderr, "Unexpected option: -%c\n", opt);  // Handle unexpected option
            exit(EXIT_FAILURE);
    }
}





/**
 * Parses command-line options and sets corresponding flags.
 *
 * @param argc: Argument count.
 * @param argv: Argument vector.
 */
static void parse_options(int argc, char *argv[])
{
    int opt;

    // Parse options using getopt()
    while ((opt = getopt(argc, argv, ":latucifd1")) != -1) 
    {
        set_option_flag(opt);  // Set flag based on parsed option
    }
}




/**
 * Lists the contents of directories passed as arguments.
 *
 * @param argc: Argument count.
 * @param argv: Argument vector.
 */
static void list_specified_directories(int argc, char *argv[])
{
    // Loop through the passed directories
    for (int i = optind; i < argc; i++) 
    {
        // Ignore options (arguments starting with '-')
        if (argv[i][0] == '-') 
        {
            continue;
        }

        printf("%s:\n", argv[i]);
        list_directory(argv[i]);  // List the specified directory

        // Print a newline between directories for formatting consistency
        if (i < argc - 1) 
        {
            printf("\n");
        }
    }
}





/*============================================================================
 ****************************  Main Function  ********************************
 ============================================================================*/ 
/**
 * Main function to parse options and list directory contents.
 *
 * @param argc: Argument count.
 * @param argv: Argument vector (array of arguments).
 * @return: Exit status.
 */
int main(int argc, char *argv[])
{
    parse_options(argc, argv);  // Parse command-line options

    // If no directory is passed, list the current working directory's entries
    if (optind == argc) 
    {
        list_directory(".");  // List current directory
    } 
    else 
    {
        list_specified_directories(argc, argv);  // List passed directories
    }

    return 0;
}

