 /*===================================================================================
 * @file           : Helper.h
 * @author         : Ali Mamdouh
 * @brief          : Header of Helper file.
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
 *===================================================================================
 * 
 *===================================================================================
 */


/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#ifndef _UTILS_H_
#define _UTILS_H_
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





/*============================================================================
 ********************************  Macros  ***********************************
 ============================================================================*/ 
/* Text Colors */

/**
 * @brief Color code for executable files.
 * 
 * This color is used to display executable files in green.
 */
#define GREEN                   "\033[1;32m"   

/**
 * @brief Color code for broken symlinks or missing files.
 * 
 * This color is used to display broken symlinks or missing files in red.
 */
#define RED                     "\033[1;31m"   

/**
 * @brief Color code for broken symlinks with red text and black background.
 * 
 * This color is used for highlighting broken symlinks with a red text and black background.
 */
#define RED_HIGHLIGHT           "\033[1;31;40m" 

/**
 * @brief Color code for white text on a red background.
 * 
 * This color is used for displaying text in white with a red background.
 */
#define WHITE_TEXT_RED_HIGHLIGHT          "\033[1;37;41m" 

/**
 * @brief Color code for black text on a yellow background.
 * 
 * This color is used for displaying text in black with a yellow background.
 */
#define BLACK_TEXT_YELLOW_HIGHLIGHT       "\033[1;30;43m" 

/**
 * @brief Color code for device files (block/character devices).
 * 
 * This color is used to display device files in yellow.
 */
#define YELLOW                  "\033[1;33m"   

/**
 * @brief Color code for directories.
 * 
 * This color is used to display directories in blue.
 */
#define BLUE                    "\033[1;34m"   

/**
 * @brief Color code for symlinks.
 * 
 * This color is used to display symbolic links in magenta.
 */
#define MAGNETA                 "\033[1;35m"   

/**
 * @brief Color code for special files (e.g., FIFO, sockets).
 * 
 * This color is used to display special files in cyan.
 */
#define CYAN                    "\033[1;36m"   

/**
 * @brief Color code for regular files with no special attributes.
 * 
 * This color is used to display regular files in white.
 */
#define WHITE                   "\033[1;37m"   

/**
 * @brief Color code for bold text.
 * 
 * This color is used to display text in bold, often used for highlighting.
 */
#define BOLD                    "\033[1m"      

/**
 * @brief Color code for underlined text.
 * 
 * This color is used to display text with an underline for emphasis.
 */
#define UNDERLINE               "\033[4m"      

/**
 * @brief Color code to reset to default text color.
 * 
 * This color is used to reset text color to the terminal's default.
 */
#define RESET_COLOR             "\033[0m"      

/**
 * @brief Color code for bold blue text.
 * 
 * This color is used to display text in bold blue.
 */
#define BOLD_BLUE               "\033[1;34m"

/**
 * @brief Color code for bold cyan text.
 * 
 * This color is used to display text in bold cyan.
 */
#define BOLD_CYAN               "\033[1;36m"

/**
 * @brief Color code for bold green text.
 * 
 * This color is used to display text in bold green.
 */
#define BOLD_GREEN              "\033[1;32m"

/**
 * @brief Color code for bold red text.
 * 
 * This color is used to display text in bold red.
 */
#define BOLD_RED                "\033[1;31m"


/**
 * @brief Color code for yellow text on a black background.
 * 
 * This color is used to display text in yellow with a black background.
 */
#define YELLOW_BLACK_BG "\033[33;40m"

/**
 * @brief Color code for bold red text on a black background.
 * 
 * This color is used to display text in bold red with a black background.
 */
#define BOLD_RED_BLACK_BG "\033[1;31;40m"

/**
 * @brief Color code for white text on a red background.
 * 
 * This color is used to display text in white with a red background.
 */
#define WHITE_RED_BG "\033[37;41m"

/**
 * @brief Color code for black text on a yellow background.
 * 
 * This color is used to display text in black with a yellow background.
 */
#define BLACK_YELLOW_BG "\033[30;43m"

/**
 * @brief Color code for white text on a blue background.
 * 
 * This color is used to display text in white with a blue background.
 */
#define WHITE_BLUE_BG "\033[37;44m"

/**
 * @brief Color code for blue text on a green background.
 * 
 * This color is used to display text in blue with a green background.
 */
#define BLUE_GREEN_BG "\033[34;42m"

/**
 * @brief Color code for black text on a green background.
 * 
 * This color is used to display text in black with a green background.
 */
#define BLACK_GREEN_BG "\033[30;42m"

/**
 * @brief Color code to reset text to default (uncolored).
 * 
 * This color is used to remove any applied color or formatting.
 */
#define UNCOLORED    "\033[0m"

/* File Type Color Codes */

/**
 * @brief Color code for FIFO or named pipe.
 * 
 * This color is used to display FIFO or named pipe files in cyan.
 */
#define NAMED_PIPE              CYAN           

/**
 * @brief Color code for compressed files.
 * 
 * This color is used to display compressed files in bold red.
 */
#define COMPRESSED_FILE         BOLD_RED          

/**
 * @brief Color code for character special files.
 * 
 * This color is used to display character special files in yellow.
 */
#define CHARACTER_SPECIAL_FILE  YELLOW          

/**
 * @brief Color code for directories.
 * 
 * This color is used to display directories in blue.
 */
#define DIRECTORY               BLUE    

/**
 * @brief Color code for block special files.
 * 
 * This color is used to display block special files in yellow.
 */
#define BLOCK_SPECIAL_FILE      YELLOW       

/**
 * @brief Color code for regular files.
 * 
 * This color is used to display regular files in white.
 */
#define REGULAR_FILE            WHITE   

/**
 * @brief Color code for symbolic links.
 * 
 * This color is used to display symbolic links in cyan.
 */
#define SOFT_LINK               CYAN   

/**
 * @brief Color code for sockets.
 * 
 * This color is used to display sockets in cyan.
 */
#define SOCKET                  CYAN   

/**
 * @brief Color code for executable files.
 * 
 * This color is used to display executable files in green.
 */
#define EXECUTABLE_FILE         GREEN   

/**
 * @brief Identifier for unknown file types.
 * 
 * This value is used to indicate an unknown file type.
 */
#define UNKNOWN_TYPE            -1

/**
 * @brief Identifier for broken links.
 * 
 * This value is used to indicate broken links.
 */
#define BROKEN_LINK             0

/**
 * @brief Identifier for proper links.
 * 
 * This value is used to indicate proper (valid) links.
 */
#define PROPER_LINK             1

#ifndef S_ISVTX
#define S_ISVTX 01000
#endif






/*============================================================================
 ********************************  Macros  ***********************************
 ============================================================================*/ 
int CompareFileNamesAlphabetically(const void *p1, const void *p2);
int CompareFileModificationTimes(const void *p1, const void *p2);
int CompareFileAccessTimes(const void *p1, const void *p2);
int CompareFileChangeTimes(const void *p1, const void *p2);
int CheckSymbolicLinkTarget(const char *path);
void GetFilePermessions( char* str , struct stat buf);
void PrintEntry(const char *Entry, struct stat buf , char* path);
void PrintEntry_LongFormat( struct stat buf , const char *file_name , char* path );

#endif