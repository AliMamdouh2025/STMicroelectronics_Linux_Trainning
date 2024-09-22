 /*===================================================================================
 * @file           : GPT_Parsing.h
 * @author         : Ali Mamdouh
 * @brief          : header of GPT_Parsing
 * @Reviewer       : Eng Reda
 * @Version        : 2.0.0
 *===================================================================================
 * 
 *===================================================================================
 */



#ifndef _GPT_H_
#define _GPT_H_


/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include <stdio.h>      // Provides functions for input and output operations (e.g., printf, fprintf)
#include <sys/types.h> // Defines data types used in system calls (e.g., ssize_t, off_t)
#include <sys/stat.h>  // Defines constants and functions for file status and permissions (e.g., S_IRUSR, S_IWUSR)
#include <fcntl.h>     // Provides constants and functions for file control operations (e.g., open, O_RDONLY)
#include <unistd.h>    // Provides access to the POSIX operating system API (e.g., read, write, lseek, close)
#include <inttypes.h>  // Provides integer types with specified widths and formatting (e.g., int32_t, PRIu32)
#include <string.h>    // Provides functions for string manipulation (e.g., memcpy, memset, strcmp)
#include <stdbool.h>   // Defines the boolean type and values (e.g., bool, true, false)





/*============================================================================
 *******************************  Macros  ************************************
 ============================================================================*/ 

/**
 * Defines the size of a sector in bytes.
 *
 * The sector size is a fundamental unit of storage, representing the smallest addressable unit on a storage 
 * device. In this context, it is set to 512 bytes, which is a common sector size for hard drives and SSDs.
 */
#define SECTOR_SIZE                 512  // Size of a sector in bytes

/**
 * Specifies the Logical Block Address (LBA) for the GPT (GUID Partition Table) header.
 *
 * The GPT header is located at LBA 1 in the GPT layout. This is the default location where the GPT header 
 * can be found after the protective MBR.
 */
#define GPT_HEADER_LBA              1  // LBA of the GPT header

/**
 * Defines the LBA where the GPT partition entry array begins.
 *
 * The GPT partition entry array starts at LBA 2 in the GPT layout. This is where the entries for each 
 * partition in the GPT are stored.
 */
#define GPT_ENTRY_ARRAY_LBA         2  // LBA of the GPT partition entry array

/**
 * Specifies the size of each GPT partition entry in bytes.
 *
 * Each entry in the GPT partition entry array has a fixed size of 128 bytes. This defines the structure of 
 * each partition entry within the GPT.
 */
#define GPT_ENTRY_SIZE              128  // Size of each GPT partition entry in bytes

/**
 * Defines the number of GPT partition entries.
 *
 * The GPT format supports a maximum of 128 partition entries, allowing up to 128 partitions to be defined 
 * in a GPT partition table.
 */
#define GPT_ENTRIES_NUM             128  // Number of GPT partition entries

/**
 * Represents the signature used to identify GPT (GUID Partition Table).
 *
 * The GPT signature is a specific value used to validate and recognize the GPT format. The signature value 
 * is 0xEE, indicating that the partition table uses the GPT format.
 */
#define GPT_SIGNATURE               0xEE  // GPT signature value

/**
 * Specifies the length of the string used to represent GPT types.
 *
 * The GPT type string length is set to 36 characters. This is the length required for representing GPT 
 * partition type GUIDs as strings.
 */
#define GPT_TYPE_STRING_LENGTH      36  // Length of the GPT type string in characters

/**
 * Defines the size of a GUID (Globally Unique Identifier) in bytes.
 *
 * GUIDs are used in GPT to uniquely identify partition types and other data. A GUID is 16 bytes in size.
 */
#define GUID_SIZE                   16  // Size of the GUID in bytes

/**
 * Specifies the length of a GUID string, including the null terminator.
 *
 * The GUID string length includes 36 characters for the GUID itself plus a null terminator, making the total 
 * length 37 characters.
 */
#define GUID_STR_LEN                37  // Length of the GUID string (including null terminator)

/**
 * Defines the length of a GUID string without the null terminator.
 *
 * The GUID string length without the null terminator is 36 characters. This represents the GUID in its string 
 * format.
 */
#define GUID_LEN                    36  // Length of a GUID string (without null terminator)

/**
 * Represents a constant value used to indicate a specific GUID match condition.
 *
 * This constant is used in comparisons or checks related to GUID string matching. The value of 0 typically signifies 
 * a successful string match
 */
#define GUID_STRING_MATCH           0  // Value indicating a GUID string match








/*============================================================================
 *************************  Data types Declerations  *************************
 ============================================================================*/
/**
 * Represents a partition entry in the GUID Partition Table (GPT).
 *
 * This structure holds the key information about each partition in a GPT,
 * including GUIDs for the partition and type, logical block addresses, attributes,
 * and a partition name. GPT allows for a more flexible and modern partitioning scheme
 * compared to MBR, supporting larger disks and more partitions.
 *
 * Fields:
 * 
 * @param type_guid: 
 *        A 16-byte GUID that identifies the type of the partition. This GUID
 *        is used to specify the file system or operating system type associated
 *        with the partition. For example:
 *        - `0x0C12F5DD-1B35-4827-AB1E-85A8F7D6F920`: Linux filesystem.
 *        - `0xEBD0A0A2-B9E5-4433-87C0-68B6B72699C7`: Microsoft basic data.
 * 
 * @param partition_guid: 
 *        A 16-byte GUID that uniquely identifies the partition itself. This GUID 
 *        is used to differentiate each partition in the GPT and ensure global uniqueness.
 * 
 * @param starting_lba: 
 *        A 64-bit value representing the starting Logical Block Address (LBA) of the partition. 
 *        LBA is a method of addressing blocks on a disk, and this field specifies the offset 
 *        from the start of the disk where the partition begins.
 * 
 * @param ending_lba: 
 *        A 64-bit value representing the ending Logical Block Address (LBA) of the partition. 
 *        This field specifies the offset from the start of the disk where the partition ends, 
 *        thereby defining the partition's extent on the disk.
 * 
 * @param attributes: 
 *        A 64-bit value containing various attributes associated with the partition. 
 *        These attributes can indicate special features or properties of the partition, 
 *        such as whether it is read-only or hidden.
 * 
 * @param partition_name: 
 *        A 72-byte array (36 UTF-16 characters) representing the name of the partition. 
 *        This field can be used to provide a human-readable label for the partition.
 */
typedef struct {
    uint8_t type_guid[16];        /**< GUID identifying the type of the partition. */
    uint8_t partition_guid[16];   /**< GUID uniquely identifying the partition. */
    uint64_t starting_lba;        /**< Logical Block Address of the first sector of the partition. */
    uint64_t ending_lba;          /**< Logical Block Address of the last sector of the partition. */
    uint64_t attributes;          /**< Attributes of the partition (e.g., read-only, hidden). */
    uint16_t partition_name[36];  /**< Name of the partition in UTF-16 encoding. */
} GPT_PartitionEntry;





/**
 * Represents the type of a partition in the GUID Partition Table (GPT).
 *
 * This structure is used to define and describe the type of partitions based on 
 * their GUID prefixes and human-readable names. It helps in categorizing partitions 
 * according to the type of data they hold or the operating system they are used with.
 *
 * Fields:
 * 
 * @param guid_prefix: 
 *        A pointer to a string that represents the GUID prefix associated with 
 *        the partition type. This prefix is used to identify and categorize 
 *        partitions based on their type.
 * 
 * @param name: 
 *        A pointer to a string that provides a human-readable name for the partition 
 *        type. This name is used to display the type of partition in a more user-friendly 
 *        format, making it easier for users to understand the partition's purpose.
 */
typedef struct {
    const char *guid_prefix; /**< GUID prefix representing the partition type. */
    const char *name;        /**< Human-readable name of the partition type. */
} GPT_PartitionType;






/*============================================================================
 *************************  Functions API Decleration  ***********************
 ============================================================================*/
bool convert_guid_to_string(const unsigned char *guid, char *guid_str);
const char* get_partition_type(const char *type_guid);
void GPT_print_partition_info(const char *device, int index, GPT_PartitionEntry *entry);

#endif
