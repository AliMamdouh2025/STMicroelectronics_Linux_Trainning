 /*===================================================================================
 * @file           : Helper.c
 * @author         : Ali Mamdouh
 * @brief          : Contains helper functions used by Option_Handler files.
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
 *===================================================================================
 * 
 *===================================================================================
 */




/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include "Helper.h"
#include "Option_Handler.h"
#include <string.h>
#include <ctype.h>





/*============================================================================
 *********************  Global Variables Declerations  ***********************
 ============================================================================*/ 

extern int OptionsFlags[9];






/*============================================================================
 ******************************  Functions  **********************************
 ============================================================================*/ 
/**
 * Converts all characters in a string to lowercase.
 *
 * This function iterates through the input string and converts each character to its 
 * lowercase equivalent using the `tolower` function. The conversion is performed in-place,
 * meaning that the original string is modified directly. This is useful for standardizing 
 * string case for consistent comparisons or sorting operations.
 *
 * @param str: A pointer to the string that will be converted to lowercase. 
 *             The string is modified in place, so the original content of `str` 
 *             will be changed after the function execution.
 *
 * @return: Void.
 */
static void to_lowercase(char *str) 
{
    while (*str) 
    {
        *str = tolower((unsigned char) *str);
        str++;
    }
}



/**
 * Compares two file names alphabetically for sorting.
 *
 * This function compares two file names to determine their alphabetical order. 
 *
 * The function is used in sorting operations to provide consistent ordering of file names.
 *
 * @param p1: A pointer to the first file name (pointer to a pointer to char).
 *            The file name is compared against the second file name.
 * @param p2: A pointer to the second file name (pointer to a pointer to char).
 *            The file name is compared against the first file name.
 *
 * @return: A negative value if the first file name is less than the second.
 *          Zero if the file names are identical.
 *          A positive value if the first file name is greater than the second.
 */
int CompareFileNamesAlphabetically(const void *p1, const void *p2)
{
    // Duplicate the two strings to be able to make processing on them
    char* lowerCasePtr1 = strdup(*(const char **) p1);
    char* lowerCasePtr2 = strdup(*(const char **) p2);

    // Convert all characters to lowercase for case-insensitive comparison
    to_lowercase(lowerCasePtr1);
    to_lowercase(lowerCasePtr2);

    // Compare the lowercase file names alphabetically
    int result = strcmp(lowerCasePtr1, lowerCasePtr2);

    // Free duplicated strings from heap
    free(lowerCasePtr1);
    free(lowerCasePtr2);

    return result;
}








/**
 * Compares two files based on their modification times for sorting.
 *
 * This function retrieves the modification times of two files and compares them to determine
 * their order. The comparison is done in descending order, meaning more recent modification times
 * come first. If there is an error retrieving the file statistics, the function treats the files
 * as equal. This function is used for sorting files based on their modification time.
 *
 * @param p1: A pointer to the first file name (pointer to a pointer to char). The file name 
 *            is used to retrieve and compare the file's modification time.
 * @param p2: A pointer to the second file name (pointer to a pointer to char). The file name 
 *            is used to retrieve and compare the file's modification time.
 *
 * @return: A negative value if the first file's modification time is greater (more recent).
 *          Zero if the modification times are the same.
 *          A positive value if the first file's modification time is less (older).
 */
int CompareFileModificationTimes(const void *p1, const void *p2)
{
    struct stat buf1, buf2;

    /** Cast the arguments back to pointers to char (file names) */
    const char *file1 = *(const char **) p1;
    const char *file2 = *(const char **) p2;

    /** Get file stats for each file */
    if (lstat(file1, &buf1) < 0) 
    {
        printf("stat error for file1: %s", file1);
        return 0; /** Error case, treat files as equal */
    }
    if (lstat(file2, &buf2) < 0) 
    {
        perror("stat error for file2");
        return 0; /** Error case, treat files as equal */
    }

    /** Compare modification times (st_mtime) */
    if (buf1.st_mtime > buf2.st_mtime)
        return -1;  /** Sort in descending order (most recent first) */
    else if (buf1.st_mtime < buf2.st_mtime)
        return 1;   /** Sort in descending order */
    else
        return 0;   /** Modification times are the same */
}



/**
 * Compares two files based on their access times for sorting.
 *
 * This function retrieves the access times of two files and compares them to determine
 * their order. The comparison is done in descending order, meaning more recent access times
 * come first. If there is an error retrieving the file statistics, the function treats the files
 * as equal. This function is used for sorting files based on their access time.
 *
 * @param p1: A pointer to the first file name (pointer to a pointer to char). The file name 
 *            is used to retrieve and compare the file's access time.
 * @param p2: A pointer to the second file name (pointer to a pointer to char). The file name 
 *            is used to retrieve and compare the file's access time.
 *
 * @return: A negative value if the first file's access time is greater (more recent).
 *          Zero if the access times are the same.
 *          A positive value if the first file's access time is less (older).
 */
int CompareFileAccessTimes(const void *p1, const void *p2)
{
    struct stat buf1, buf2;

    /** Cast the arguments back to pointers to char (file names) */
    const char *file1 = *(const char **) p1;
    const char *file2 = *(const char **) p2;

    /** Get file stats for each file */
    if (lstat(file1, &buf1) < 0) 
    {
        perror("stat error for file1");
        return 0; /** Error case, treat files as equal */
    }
    if (lstat(file2, &buf2) < 0) 
    {
        perror("stat error for file2");
        return 0; /** Error case, treat files as equal */
    }

    /** Compare access times (st_atime) */
    if (buf1.st_atime > buf2.st_atime)
        return -1;  /** Sort in descending order (most recent first) */
    else if (buf1.st_atime < buf2.st_atime)
        return 1;   /** Sort in descending order */
    else
        return 0;   /** Access times are the same */
}



/**
 * Compares two files based on their change times for sorting.
 *
 * This function retrieves the change times of two files and compares them to determine
 * their order. The comparison is done in descending order, meaning more recent change times
 * come first. If there is an error retrieving the file statistics, the function treats the files
 * as equal. This function is used for sorting files based on their change time.
 *
 * @param p1: A pointer to the first file name (pointer to a pointer to char). The file name 
 *            is used to retrieve and compare the file's change time.
 * @param p2: A pointer to the second file name (pointer to a pointer to char). The file name 
 *            is used to retrieve and compare the file's change time.
 *
 * @return: A negative value if the first file's change time is greater (more recent).
 *          Zero if the change times are the same.
 *          A positive value if the first file's change time is less (older).
 */
int CompareFileChangeTimes(const void *p1, const void *p2)
{
    struct stat buf1, buf2;

    /** Cast the arguments back to pointers to char (file names) */
    const char *file1 = *(const char **) p1;
    const char *file2 = *(const char **) p2;

    /** Get file stats for each file */
    if (lstat(file1, &buf1) < 0) 
    {
        perror("stat error for file1");
        return 0; /** Error case, treat files as equal */
    }
    if (lstat(file2, &buf2) < 0) 
    {
        perror("stat error for file2");
        return 0; /** Error case, treat files as equal */
    }

    /** Compare change times (st_ctime) */
    if (buf1.st_ctime > buf2.st_ctime)
        return -1;  /** Sort in descending order (most recent first) */
    else if (buf1.st_ctime < buf2.st_ctime)
        return 1;   /** Sort in descending order */
    else
        return 0;   /** Change times are the same */
}








/**
 * Checks if a given path is a symbolic link and determines if it is broken or valid.
 *
 * This function checks if the specified path points to a symbolic link and whether
 * the symbolic link is broken or not. If the path does not exist (ENOENT error), it is
 * considered a broken link. If there is another error retrieving the file status, it reports
 * the error using `perror`. If the path is valid and the symbolic link exists, it is treated as
 * a proper link.
 *
 * @param path: The path to be checked. This is a string representing the file path to the
 *              symbolic link that needs to be evaluated.
 *
 * @return: `BROKEN_LINK` if the path is a symbolic link that is broken or does not exist.
 *          `PROPER_LINK` if the path is a valid symbolic link.
 */
int CheckSymbolicLinkTarget(const char *path) 
{
    struct stat buf;
    if (stat(path, &buf) == -1) 
    {
        if (errno == ENOENT) 
        {
            return BROKEN_LINK;
        } 
        else 
        {
            perror("stat error");
        }
    }

    return PROPER_LINK;
}



/**
 * Selects and prints color based on the file mode.
 *
 * This function determines the type of the file based on its mode (e.g., regular file, directory,
 * symbolic link) and selects an appropriate color or format to print. It also checks if the file
 * is executable, compressed, or if it is a special file (e.g., block, character special file).
 * If the file is a symbolic link, it further checks if the link is broken or proper.
 *
 * @param FileName: The name of the file, used to determine if it is a compressed file based
 *                  on its extension.
 * @param buf: The `stat` structure containing information about the file, including its mode.
 * @param path: The path to the file, used to check if a symbolic link is broken.
 *
 * @return: Void.
 */
static void Select_Color_BasedOnFileMode(char *FileName, struct stat buf, char* path)
{
    /* Choose Output color based on type of files */
    if (buf.st_mode & S_ISUID) 
    {
        printf(WHITE_TEXT_RED_HIGHLIGHT);
    } 
    else if (buf.st_mode & S_ISGID) 
    {
        printf(BLACK_TEXT_YELLOW_HIGHLIGHT);
    }
    /** Check if it's an executable regular file */
    else if (S_ISREG(buf.st_mode) && (buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) 
    {
        /** Executable file */
        printf(EXECUTABLE_FILE);
    }
    /** Check if it's a compressed file */
    else if (S_ISREG(buf.st_mode) && (strstr(FileName, ".zip") || strstr(FileName, ".tar") || strstr(FileName, ".7z"))) 
    {
        /** Compressed file */
        printf(COMPRESSED_FILE);
    }
    /** Check if it's a regular file */
    else if (S_ISREG(buf.st_mode)) 
    {
        /** Regular file */
        printf(REGULAR_FILE);
    }
    /** Check if it's a directory */
    else if (S_ISDIR(buf.st_mode)) 
    {
        /** Directory */
        printf(DIRECTORY);
    }
    /** Check if it's a character special file */
    else if (S_ISCHR(buf.st_mode)) 
    {
        /** Character special file (e.g., terminal devices) */
        printf(CHARACTER_SPECIAL_FILE);
    }
    /** Check if it's a block special file */
    else if (S_ISBLK(buf.st_mode)) 
    {
        /** Block special file (e.g., disk devices) */
        printf(BLOCK_SPECIAL_FILE);
    }
    /** Check if it's a FIFO or named pipe */
    else if (S_ISFIFO(buf.st_mode)) 
    {
        /** FIFO or named pipe */
        printf(NAMED_PIPE);
    }
    /** Check if it's a socket */
    else if (S_ISSOCK(buf.st_mode)) 
    {
        /** Socket */
        printf(SOCKET);
    }
    /** Check if it's a symbolic link */
    else if (S_ISLNK(buf.st_mode)) 
    {
        if (CheckSymbolicLinkTarget(path) == BROKEN_LINK) 
        {
            /** Broken link => color is red */
            printf(RED_HIGHLIGHT);
        } 
        else 
        {
            /** Proper symbolic link */
            printf(SOFT_LINK);
        }
    }
    /** Default case for unrecognized file type */
    else 
    {
        /** Default case (unrecognized file type) */
        printf(WHITE);
    }
}



/**
 * Prints the entry name and additional details based on file mode and options.
 *
 * This function prints the name of the file or directory, with color or format applied based
 * on the file type and its mode. If the long format option is enabled and the file is a symbolic
 * link, it also prints the target of the symbolic link. The color is reset to default after
 * printing. Memory allocated for the file name is freed after use.
 *
 * @param Entry: The name of the file or directory entry to be printed.
 * @param buf: The `stat` structure containing information about the file, including its mode.
 * @param path: The path to the file, used to check if a symbolic link is broken and to read
 *              the link target if needed.
 *
 * @return: Void.
 */
void PrintEntry(const char *Entry, struct stat buf, char* path)
{
    /* To make processing on file name with String Library with more free */
    char* FileName = strdup(Entry);

    /* Choose Output color based on type of files */
    Select_Color_BasedOnFileMode(FileName, buf, path);

    /** Print the entry name */
    printf("%s", FileName);

    /** Check if long format option is set and if it's a symbolic link => print the target file that symbolic link points to */
    if (OptionsFlags[LONG_FORMAT_OPTION_l] && S_ISLNK(buf.st_mode)) 
    {
        char link_target[MAX_PATH_LENGTH];

        ssize_t len = readlink(path, link_target, sizeof(link_target) - 1);
        if (len != -1) 
        {
            /** Null-terminate the string */
            link_target[len] = '\0';

            /** Print the symbolic link target */
            printf(" -> %s", link_target);
        } 
        else 
        {
            /** Error reading symbolic link */
            perror("Error reading symbolic link");
        }
    }

    /* Reset color to default after printing */
    printf(RESET_COLOR);

    /* Free duplicated file name from heap */
    free(FileName);
}









/**
 * Retrieves the file permissions and formats them into a string.
 *
 * This function populates a string with the file permissions in the standard
 * `ls -l` format. The string represents the file type and permissions for
 * the owner, group, and others, including special permissions (Set UID, Set GID,
 * and sticky bit). The permissions are represented as characters:
 * - 'd' for directory
 * - 'l' for symbolic link
 * - '-' for regular file
 * - 'r' for read
 * - 'w' for write
 * - 'x' for execute
 * - 's' for Set UID or Set GID with execute permissions
 * - 'S' for Set UID or Set GID without execute permissions
 * - 't' for sticky bit with execute permissions
 * - 'T' for sticky bit without execute permissions
 *
 * @param str: A string of length at least 11 characters, which will be populated
 *             with the formatted permissions. It will include the file type and
 *             permissions for the owner, group, and others, ending with a null
 *             terminator.
 * @param buf: A `stat` structure containing file status information, including
 *             the file mode.
 *
 * @return: Void.
 */
void GetFilePermessions(char* str, struct stat buf)
{
    int mode = buf.st_mode;

    // Check if the file is executable
    if (S_ISDIR(mode)) str[0] = 'd'; // Directory
    else if (S_ISLNK(mode)) str[0] = 'l'; // Symbolic link
    else if (S_ISREG(mode)) str[0] = '-'; // Regular file
    
    /* Owner permissions */
    str[1] = (mode & S_IRUSR) ? 'r' : '-'; // Owner read
    str[2] = (mode & S_IWUSR) ? 'w' : '-'; // Owner write
    if (mode & S_ISUID) 
    {
        str[3] = (mode & S_IXUSR) ? 's' : 'S'; // Set UID and execute permissions
    } 
    else 
    {
        str[3] = (mode & S_IXUSR) ? 'x' : '-'; // Owner execute
    }

    /* Group permissions */
    str[4] = (mode & S_IRGRP) ? 'r' : '-'; // Group read
    str[5] = (mode & S_IWGRP) ? 'w' : '-'; // Group write
    if (mode & S_ISGID) 
    {
        str[6] = (mode & S_IXGRP) ? 's' : 'S'; // Set GID and execute permissions
    } 
    else 
    {
        str[6] = (mode & S_IXGRP) ? 'x' : '-'; // Group execute
    }

    /* Others' permissions */
    str[7] = (mode & S_IROTH) ? 'r' : '-'; // Others read
    str[8] = (mode & S_IWOTH) ? 'w' : '-'; // Others write
    if (mode & S_ISVTX) 
    {
        str[9] = (mode & S_IXOTH) ? 't' : 'T'; // Sticky bit and others' execute permissions
    } 
    else 
    {
        str[9] = (mode & S_IXOTH) ? 'x' : '-'; // Others execute
    }

    str[10] = '\0'; // Null-terminate the string
}



/**
 * Prints file information in a long format.
 *
 * This function prints detailed information about a file or directory in a
 * long format, similar to the `ls -l` command. The information includes
 * the file's inode number, permissions, number of hard links, owner name,
 * group name, file size, and modification or access time. The file name is
 * printed at the end. The function also considers options to show inode number
 * and access or change time based on the flags set in `OptionsFlags`.
 *
 * @param buf: A `stat` structure containing file status information, including
 *             permissions, number of links, owner, group, size, and time.
 * @param file_name: The name of the file or directory to be printed.
 * @param path: The path to the file, used to determine the access or modification
 *              time and to read symbolic link targets if needed.
 *
 * @return: Void.
 */
void PrintEntry_LongFormat(struct stat buf, const char *file_name, char* path)
{
    if (OptionsFlags[SHOW_INODE_OPTION_i]) 
    {
        printf("%ld  ", buf.st_ino);
    }

    char str[11];
    strcpy(str, "----------");

    GetFilePermessions(str, buf);

    // Print file permissions
    printf("%3s ", str);

    // Number of hard links (right-aligned with a width of 3)
    printf("%3ld ", buf.st_nlink);

    // Owner name (left-aligned with a width of 8)
    struct passwd *pwd = getpwuid(buf.st_uid);
    printf("%-8s ", pwd->pw_name);

    // Group name (left-aligned with a width of 8)
    struct group *grp = getgrgid(buf.st_gid);
    printf("%-2s ", grp->gr_name);

    // File size (right-aligned with a width of 8)
    printf("%8ld ", buf.st_size);

    /* Time */
    if (OptionsFlags[ACCESS_TIME_OPTION_u]) 
    {
        char *time_str = ctime(&buf.st_atime);
        time_str[strlen(time_str) - 1] = '\0'; // Remove the newline
        printf(" %s ", time_str);            
    }
    else if (OptionsFlags[CHANGE_TIME_OPTION_c]) 
    {
        char *time_str = ctime(&buf.st_ctime);
        time_str[strlen(time_str) - 1] = '\0'; // Remove the newline
        printf(" %s ", time_str);            
    }
    else 
    {
        /* Default: Modification time */
        char *time_str = ctime(&buf.st_mtime);
        time_str[strlen(time_str) - 1] = '\0'; // Remove the newline
        printf(" %s ", time_str);
    }
    
    // File name (left-aligned)
    PrintEntry(file_name, buf, path);
}
