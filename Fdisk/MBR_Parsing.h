/**
 *===================================================================================
 * @file           : MBR_Parsing.h
 * @author         : Ali Mamdouh
 * @brief          : header of MBR_Parsing
 * @Reviewer       : Eng Reda
 * @Version        : 2.0.0
 *===================================================================================
 * 
 *===================================================================================
 */


#ifndef _MBR_H_
#define _MBR_H_



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
#include <stdlib.h>    // Provides functions for memory allocation and process control (e.g., malloc, free, exit)






/*============================================================================
 ********************************* Macros ************************************
 ============================================================================*/
/**
 * Defines the size of a sector in bytes.
 *
 * This macro represents the standard size of a disk sector, which is 512 bytes.
 * A sector is the smallest addressable unit of data on most storage devices,
 * such as hard drives, SSDs, and other block storage systems. This value is 
 * commonly used in partition management, disk I/O operations, and file systems.
 */
#define SECTOR_SIZE                 512  // 512 bytes per sector

/**
 * Represents the partition type for CHS-based extended partitions in the MBR.
 *
 * The partition type `0x05` identifies an extended partition that uses CHS 
 * (Cylinder-Head-Sector) addressing. Extended partitions allow the creation 
 * of logical partitions when more than four partitions are needed, which is 
 * the limit for primary partitions in an MBR.
 *
 * This partition type is used on older systems that rely on CHS for addressing,
 * although modern systems primarily use LBA.
 */
#define CHS_EXTENDED_PARTITION      0x05  // CHS-based extended partition type

/**
 * Represents the partition type for LBA-based extended partitions in the MBR.
 *
 * LBA extended partitions allow the creation of logical partitions, similar to CHS extended partitions, but 
 * using LBA for addressing.
 *
 * This type is preferred for modern extended partitions.
 */
#define LBA_EXTENDED_PARTITION      0x0F  // LBA-based extended partition type

/**
 * Represents the partition type for Linux extended partitions.
 *
 * The partition type `0x85` is used to identify an extended partition specifically
 * for Linux. Like other extended partitions, it allows the creation of logical 
 * partitions. This type is commonly used in Linux systems where multiple partitions
 * are needed beyond the limit of four primary partitions.
 */
#define LINUX_EXTENDED_PARTITION    0x85  // Linux-specific extended partition type

/**
 * Defines the number of primary partitions supported in the MBR (Master Boot Record).
 *
 * The MBR format specifies that a maximum of four primary partitions can be defined. This limit includes
 * both primary partitions and extended partitions.
 *
 * Note: If more partitions are needed, one of the primary partitions can be used to create an extended 
 * partition, which in turn can contain multiple logical partitions.
 */
#define MBR_PARTITIONS_NUM          4  // Number of primary partitions in the MBR

/**
 * Represents the signature used to identify the end of the MBR (Master Boot Record) and indicate 
 * that the MBR is valid.
 *
 * The signature is a 2-byte value located at the end of the MBR. The presence of this signature signifies 
 * that the MBR is properly formatted and valid. It is used for validation during MBR parsing.
 *
 */
#define MBR_SIGNATURE               0xAA55  // MBR signature value






/*============================================================================
 *************************  Data types Declerations  *************************
 ============================================================================*/
/**
 * Represents a partition entry in the Master Boot Record (MBR).
 *
 * This structure holds the key information about each partition in an MBR,
 * including the partition status, CHS addresses, partition type, starting LBA, 
 * and the total number of sectors. An MBR can have up to 4 primary partition entries.
 *
 * Fields:
 * 
 * @param status: 
 *        The partition's boot indicator. 
 *        - `0x80`: Bootable partition.
 *        - `0x00`: Non-bootable partition.
 * 
 * @param first_chs: 
 *        The Cylinder-Head-Sector (CHS) address of the first sector of the partition.
 *        It is an array of 3 bytes that represents the physical starting location 
 *        of the partition on the disk, though it's often not used in modern systems.
 * 
 * @param partition_type: 
 *        The partition type identifier. This byte indicates the type of the partition 
 *        and helps the operating system understand how to handle the partition.
 *        Common values include:
 *        - `0x83`: Linux filesystem.
 *        - `0x07`: NTFS/HPFS/exFAT.
 *        - `0x0F`: Extended partition (LBA).
 * 
 * @param last_chs: 
 *        The Cylinder-Head-Sector (CHS) address of the last sector of the partition.
 *        It is also an array of 3 bytes, indicating the physical end of the partition 
 *        on the disk. Like `first_chs`, this is rarely used in modern systems.
 * 
 * @param lba: 
 *        The Logical Block Address (LBA) of the first sector of the partition.
 *        LBA is a modern method of addressing blocks on a disk, and this field 
 *        contains the offset from the start of the disk where the partition begins.
 * 
 * @param sector_count: 
 *        The total number of sectors in the partition. This defines the partition's 
 *        size and is used along with the `lba` to determine the partition's extent on the disk.
 */
typedef struct {
    uint8_t status;           /**< Boot status of the partition. */
    uint8_t first_chs[3];     /**< CHS address of the first sector (physical start location). */
    uint8_t partition_type;   /**< Type identifier for the partition. */
    uint8_t last_chs[3];      /**< CHS address of the last sector (physical end location). */
    uint32_t lba;             /**< Logical Block Address of the first sector of the partition. */
    uint32_t sector_count;    /**< Total number of sectors in the partition. */
} MBR_PartitionEntry;







/*============================================================================
 *************************  Functions API Decleration  ***********************
 ============================================================================*/
void MBR_print_size(uint64_t size_in_sectors);
void MBR_print_partition_info(const char *device, int index, MBR_PartitionEntry *entry, uint32_t base_lba);
void MBR_parse_ebr(int fd, const char *device, uint32_t First_EBR_LBA, int *index);

#endif
