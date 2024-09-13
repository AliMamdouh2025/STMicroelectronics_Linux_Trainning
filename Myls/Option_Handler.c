 /*===================================================================================
 * @file           : Option_Handler.c
 * @author         : Ali Mamdouh
 * @brief          : Execute ls with its diffrent options
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
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include "Helper.h"
#include "Option_Handler.h"





/*============================================================================
 **********************  Global Variables Declerations  **********************
 ============================================================================*/ 
extern int errno;
int OptionsFlags[9] = {0};





/*============================================================================
 ********************************  Functions  ********************************
 ============================================================================*/ 

/**
 * Constructs the full path for a file by combining the directory and file name.
 *
 * @param path: The buffer to store the full file path.
 * @param path_size: Size of the path buffer.
 * @param dir: Directory name.
 * @param file_name: File name to append to the directory.
 */
static void construct_file_path(char *path, size_t path_size, const char *dir, const char *file_name)
{
    snprintf(path, path_size, "%s/%s", dir, file_name);
}



/**
 * Retrieves the file status using lstat and handles error reporting.
 *
 * @param path: Path to the file.
 * @param buf: Buffer to store file statistics.
 * @return: 1 if successful, 0 if lstat fails.
 */
static int get_file_status(const char *path, struct stat *buf)
{
    if (lstat(path, buf) < 0) 
    {
        perror("Error in lstat");
        return 0;
    }
    return 1;
}





/**
 * Prints a directory entry depending on the options set (-d, -f).
 *
 * @param dir: Directory name.
 * @param buf: File statistics structure.
 */
static void print_directory_entry(const char *dir, struct stat buf)
{
    if (OptionsFlags[DISABLE_EVERYTING_OPTION_f]) 
    {
        printf("%s", dir);  // Print directory name without color
    } 
    else 
    {
        PrintEntry(dir, buf, NULL);  // Print directory name with formatting
    }
    printf("\n");
}




/**
 * Prints a file entry in basic format, handling options such as -i, -f, and -1.
 *
 * @param file_name: Name of the file.
 * @param buf: File statistics structure.
 * @param path: Full path to the file.
 */
static void print_basic_entry(const char *file_name, struct stat buf, char *path)
{
    // If -i option is used, print the inode number
    if (OptionsFlags[SHOW_INODE_OPTION_i]) 
    {
        printf("%ld  ", buf.st_ino);
    }

    // If -f option is used, print without color
    if (OptionsFlags[DISABLE_EVERYTING_OPTION_f]) 
    {
        printf("%s", file_name);
    } 
    else 
    {
        PrintEntry(file_name, buf, path);  // Print file name with formatting
    }

    // Handle new line or tab separation based on options
    if (OptionsFlags[SHOW_1_FILE_IN_LINE_OPTION_1]) 
    {
        printf("\n");  // Print each file on a new line
    } 
    else 
    {
        printf("  ");  // Separate files with a tab
    }
}






/**
 * Handles printing files in basic format, including options such as -d, -f, -i, and -1.
 *
 * @param file_names[]: Array of file names in the directory.
 * @param file_count: Number of files in the directory.
 * @param dir: Directory name to list.
 */
void Print_ls_WithoutLongFormat(char *file_names[], int file_count, const char *dir)
{
    struct stat buf;

    // Handle the case where the -d option is used (print directory name only)
    if (OptionsFlags[SHOW_DIRECTORY_ITSELF_OPTION_d]) 
    {
        if (!get_file_status(dir, &buf)) 
        {
            return;
        }

        print_directory_entry(dir, buf);
    } 
    else 
    {
        // Loop over and print each file in the directory
        for (int i = 0; i < file_count; i++) 
        {
            char path[MAX_PATH_LENGTH];

            construct_file_path(path, sizeof(path), dir, file_names[i]);

            if (!get_file_status(path, &buf)) 
            {
                continue;  // Skip files with lstat errors
            }

            print_basic_entry(file_names[i], buf, path);
        }
    }
}












/**
 * Prints a directory entry in long format.
 *
 * @param buf: File statistics structure.
 * @param file_name: Name of the file.
 * @param path: Full path to the file, can be NULL if -d option is used.
 */
static void print_long_format_entry(struct stat buf, const char *file_name, char *path)
{
    PrintEntry_LongFormat(buf, file_name, path);
    printf("\n");
}





/**
 * Handles the printing of directory entries in long format.
 *
 * @param file_names[]: Array of file names in the directory.
 * @param file_count: Number of files in the directory.
 * @param dir: Directory name to list.
 */
void Print_ls_LongFormat(char *file_names[], int file_count, const char *dir)
{
    struct stat buf;

    // Handle the case where the -d option is used
    if (OptionsFlags[SHOW_DIRECTORY_ITSELF_OPTION_d]) 
    {
        if (!get_file_status(dir, &buf)) 
        {
            return;  // Exit if lstat fails
        }

        print_long_format_entry(buf, dir, NULL);
        return;
    }

    // Loop over and print each file in the directory
    for (int i = 0; i < file_count; i++) 
    {
        char path[MAX_PATH_LENGTH];

        construct_file_path(path, sizeof(path), dir, file_names[i]);

        if (!get_file_status(path, &buf)) 
        {
            continue;  // Skip files with lstat errors
        }

        print_long_format_entry(buf, file_names[i], path);
    }
}










/**
 * Handles the behavior of command-line options that disable or alter other options.
 *
 * This function is primarily responsible for managing the effects of the `-f` option 
 * (DISABLE_EVERYTING_OPTION_f). When the `-f` option is set, it automatically adjusts 
 * other options:
 * - Enables the display of hidden files by setting the `-a` option (SHOW_HIDDEN_OPTION_a).
 * - Disables the long format listing by clearing the `-l` option (LONG_FORMAT_OPTION_l).
 *
 *
 * @param None: This function does not accept any parameters.
 *
 * @return None: This function does not return a value.
 */
static void handle_disable_option() 
{
    /* If the -f option is set, adjust other options accordingly */
    if (OptionsFlags[DISABLE_EVERYTING_OPTION_f]) 
    {
        /* Enable showing hidden files (equivalent to setting the -a option) */
        OptionsFlags[SHOW_HIDDEN_OPTION_a] = 1;

        /* Disable the long format listing (equivalent to clearing the -l option) */
        OptionsFlags[LONG_FORMAT_OPTION_l] = 0;
    }
}




/**
 * Determines whether to skip hidden files based on user-specified options.
 *
 * This function checks if a file is hidden (its name starts with a '.') and whether
 * the user has requested to show hidden files using the `-a` option. If the `-a`
 * option is not set, the function returns `1` (true) to indicate that hidden files 
 * should be skipped. Otherwise, it returns `0` (false) to include hidden files 
 * in the directory listing.
 *
 *
 * @param filename: A string representing the name of the file. The function checks 
 *                  whether this file is hidden by examining the first character of 
 *                  the file name.
 * 
 * @return: Returns `1` (true) if the file should be skipped (i.e., the file is hidden 
 *          and the `-a` option is not set). Returns `0` (false) if the file should 
 *          not be skipped (either because it is not hidden or the `-a` option is set).
 */
static int should_skip_hidden(const char *filename) 
{
    /* If the -a option is not set and the file is hidden, return true (1) */
    return (!OptionsFlags[SHOW_HIDDEN_OPTION_a] && filename[0] == '.');
}




/**
 * Stores a file name in the `file_names` array with proper memory allocation.
 *
 * This function dynamically allocates memory for a file name and stores it in the 
 * `file_names` array. It ensures that each file name is copied into the array 
 * safely, and handles memory allocation errors by printing an error message and 
 * exiting the program.
 *
 * If memory allocation fails, the function prints an error message using `perror`
 * and immediately terminates the program with an exit status of `EXIT_FAILURE`.
 *
 * @param file_names: An array of strings where each element represents a file name.
 *                    The function allocates memory for each file name and stores 
 *                    them in this array.
 * @param file_name:  A string representing the name of the file to be stored in 
 *                    the array. The function allocates enough memory to store 
 *                    this file name and copies it into the array.
 * @param file_count: A pointer to an integer that tracks the number of files in the 
 *                    `file_names` array. The function increments this count 
 *                    after adding a new file name.
 *
 * @return: Void
 */
static void store_file_name(char *file_names[], const char *file_name, int *file_count) 
{
    /* Allocate memory for the new file name */
    file_names[*file_count] = malloc(strlen(file_name) + 1);
    
    /* Check if memory allocation was successful */
    if (file_names[*file_count] == NULL) 
    {
        perror("Memory allocation failed");  // Print error message
        exit(EXIT_FAILURE);  // Exit on memory allocation failure
    }

    /* Copy the file name into the allocated memory */
    strcpy(file_names[*file_count], file_name);

    /* Increment the file count to reflect the added file */
    (*file_count)++;  
}





/**
 * Sorts the files based on the selected command-line options.
 *
 * This function sorts the array of file names based on the user's input options. 
 * Different sorting criteria are applied depending on whether the user has requested
 * sorting by modification time, access time, or change time. If none of the time-based
 * sorting options are selected, the files are sorted alphabetically by name. If the `-f`
 * option is set, sorting is disabled, and the order remains unchanged.
 *
 *
 * @param file_names: An array of strings containing the names of the files to be sorted.
 *                    The array is modified in place, and the order of the file names 
 *                    will reflect the sorting criteria after the function is executed.
 * @param file_count: The number of files in the `file_names` array. This value determines
 *                    how many entries will be sorted.
 *
 * @return: Void.
 */
static void sort_files(char *file_names[], int file_count) 
{
    /* If sorting by modification time is requested (-t option) */
    if (OptionsFlags[SORT_BY_TIME_OPTION_t]) 
    {
        /* Sort files by their modification time */
        qsort(file_names, file_count, sizeof(char *), CompareFileModificationTimes);
    } 
    /* If sorting by access time (-u and -t options) */
    else if (OptionsFlags[ACCESS_TIME_OPTION_u] && OptionsFlags[SORT_BY_TIME_OPTION_t]) 
    {
        /* Sort files by their access time */
        qsort(file_names, file_count, sizeof(char *), CompareFileAccessTimes);
    } 
    /* If sorting by change time (-c and -t options) */
    else if (OptionsFlags[CHANGE_TIME_OPTION_c] && OptionsFlags[SORT_BY_TIME_OPTION_t]) 
    {
        /* Sort files by their change time */
        qsort(file_names, file_count, sizeof(char *), CompareFileChangeTimes);
    } 
    /* If no sorting option is provided and the -f option is not set (alphabetical sorting by name) */
    else if (!OptionsFlags[DISABLE_EVERYTING_OPTION_f]) 
    {
        /* Sort files alphabetically by their name */
        qsort(file_names, file_count, sizeof(char *), CompareFileNamesAlphabetically);
    }

    /* If the -f option is set, no sorting is applied (skips sorting step) */
}





/**
 * Prints the files in the specified directory in either long format or basic format.
 *
 * This function determines whether to display the file list in long format or basic
 * format based on command-line options. If the long format option (`-l`) is enabled, 
 * it calls the `LongFormat_ls` function to display detailed file information. 
 * Otherwise, it uses the `Basic_ls` function to display the file names in a compact format.
 *
 * After listing the files in basic format, it appends a newline character for proper output formatting.
 *
 *
 * @param file_names: An array of strings containing the names of the files to be printed.
 *                    The array should be dynamically allocated and passed to this function.
 * @param file_count: The number of files in the `file_names` array. This value determines how
 *                    many entries will be processed and printed.
 * @param dir:        A string representing the directory path that contains the files to be listed.
 *                    This is used to generate the full paths to the files for metadata access.
 *
 * @return: Void. This function does not return a value, but prints the file list to the output.
 */
static void print_files(char *file_names[], int file_count, const char *dir) 
{
    /* Check if the long format option is enabled */
    if (OptionsFlags[LONG_FORMAT_OPTION_l]) 
    {
        /* Print the files in long format (detailed information) */
        Print_ls_LongFormat(file_names, file_count, dir);
    } 
    else 
    {
        /* Print the files in basic format (compact listing) */
        Print_ls_WithoutLongFormat(file_names, file_count, dir);
        
        /* Print a newline after the basic listing for proper formatting */
        printf("\n");
    }
}




/**
 * Performs a directory listing and displays file information.
 *
 * This function reads the contents of the directory specified by `dir`, processes
 * the file entries based on command-line options, and prints the directory listing 
 * in the desired format. It handles options such as sorting, skipping hidden files,
 * and memory management for storing file names.
 *
 * Key command-line options handled by this function include:
 * - Skip hidden files unless requested.
 * - Sort file names based on various sorting options.
 * - Disable all options (-f flag).
 *
 * Memory is dynamically allocated for each file name, and the function ensures that
 * all allocated memory is freed at the end of the operation.
 *
 * @param dir: A string representing the path of the directory to list.
 *             If the directory cannot be opened, an error message is printed.
 *
 * @return: Void. This function does not return a value, but prints the directory listing
 *          or an error message if the directory cannot be opened.
 */
void Execute_ls(char *dir) 
{
    struct dirent *entry;   // Pointer to hold directory entries
    DIR *dp = opendir(dir); // Open the specified directory

    // If the directory could not be opened, print an error message and return
    if (dp == NULL) 
    {
        fprintf(stderr, "Cannot open directory '%s': %s\n", dir, strerror(errno));
        return;
    }

    /* Array of pointers to hold file names dynamically allocated later */
    char *file_names[MAX_FILES];
    int file_count = 0;     // Count of valid file entries

    /* Handle command-line option (-f) to disable certain options */
    handle_disable_option();

    /* Read directory entries one by one and process valid file names */
    while ((entry = readdir(dp)) != NULL) 
    {
        /* Skip hidden files unless the option to show them is enabled */
        if (should_skip_hidden(entry->d_name)) 
        {
            continue; // Skip this entry if it's hidden and should not be displayed
        }

        /* Store the file name in the file_names array and increment the count */
        store_file_name(file_names, entry->d_name, &file_count);

        /* If the directory contains more files than allowed, issue a warning and stop processing */
        if (file_count >= MAX_FILES) 
        {
            fprintf(stderr, "Warning: Too many files in directory, only processing the first %d files.\n", MAX_FILES);
            break;
        }
    }

    /* Close the directory as it's no longer needed */
    closedir(dp);

    /* Sort the files based on command-line options (e.g., alphabetical, based on access time..etc.) */
    sort_files(file_names, file_count);

    /* Print the directory listing based on the processed file names */
    print_files(file_names, file_count, dir);

    /* Free allocated memory for each stored file name */
    for (int i = 0; i < file_count; i++) 
    {
        free(file_names[i]);
    }
}





