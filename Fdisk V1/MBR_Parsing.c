/**
 *===================================================================================
 * @file           : MBR_Parsing.c
 * @author         : Ali Mamdouh
 * @brief          : source file to parse and print MBR  partition tables
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
 *===================================================================================
 * 
 *===================================================================================
 */




/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include "MBR_Parsing.h"






/*============================================================================
 ******************************  Functions  **********************************
 ============================================================================*/
/**
 * Get the description of a partition type based on the type code.
 *
 * This function translates a partition type code from the Master Boot Record (MBR)
 * into a human-readable string description. Each type code corresponds to a specific
 * file system or partition type, as defined in the MBR partition table.
 *
 * The function uses a switch-case structure to map each type code to its respective
 * description. If the type code is not recognized, it returns "Unknown".
 *
 * @param type: The partition type code as an unsigned char. This code is defined by
 *              the MBR and represents the type of the partition.
 *
 * @return: A constant string representing the description of the partition type. 
 *          If the type code is not recognized, it returns "Unknown".
 */
const char* MBR_get_partition_type(unsigned char type) 
{
    switch (type) 
    {
        case 0x00: return "Empty";                        // No partition
        case 0x01: return "FAT12";                       // FAT12 file system
        case 0x04: return "FAT16 (small)";               // FAT16 file system (small)
        case 0x05: return "Extended";                    // Extended partition
        case 0x06: return "FAT16";                       // FAT16 file system
        case 0x07: return "NTFS or exFAT";               // NTFS or exFAT file system
        case 0x0B: return "FAT32 (CHS)";                 // FAT32 file system (CHS addressing)
        case 0x0C: return "W95 FAT32 (LBA)";                 // FAT32 file system (LBA addressing)
        case 0x0E: return "FAT16 (LBA)";                 // FAT16 file system (LBA addressing)
        case 0x11: return "Hidden FAT16";                // Hidden FAT16 file system
        case 0x12: return "Compaq Diagnostics";          // Compaq Diagnostics
        case 0x14: return "FAT16 (large)";               // FAT16 file system (large)
        case 0x16: return "Hidden FAT16 (LBA)";          // Hidden FAT16 file system (LBA addressing)
        case 0x1B: return "Hidden FAT32 (CHS)";          // Hidden FAT32 file system (CHS addressing)
        case 0x1C: return "Hidden FAT32 (LBA)";          // Hidden FAT32 file system (LBA addressing)
        case 0x1E: return "Hidden FAT16 (LBA)";          // Hidden FAT16 file system (LBA addressing)
        case 0x20: return "Dynamic Disk";                // Dynamic Disk partition
        case 0x27: return "Hidden NTFS/HPFS";            // Hidden NTFS or HPFS file system
        case 0x39: return "Plan 9";                      // Plan 9 partition
        case 0x3C: return "PartitionMagic";              // PartitionMagic partition
        case 0x80: return "Linux Swap";                  // Linux Swap partition
        case 0x81: return "Linux";                       // Linux file system
        case 0x82: return "Linux Swap / Solaris";        // Linux Swap or Solaris partition
        case 0x83: return "Linux";                       // Linux file system
        case 0x84: return "OS/2 hidden";                 // OS/2 hidden partition
        case 0x85: return "Linux Extended";              // Linux Extended partition
        case 0x86: return "NTFS volume set";             // NTFS volume set
        case 0x87: return "NTFS volume set (no formatting)"; // NTFS volume set (no formatting)
        case 0x8A: return "OS/2 Boot Manager";           // OS/2 Boot Manager partition
        case 0x8B: return "OS/2 Boot Manager";           // OS/2 Boot Manager partition
        case 0xA0: return "IBM ThinkPad";                // IBM ThinkPad partition
        case 0xA5: return "FreeBSD/NetBSD";              // FreeBSD or NetBSD partition
        case 0xA6: return "OpenBSD";                     // OpenBSD partition
        case 0xA8: return "Mac OS X";                    // Mac OS X partition
        case 0xB1: return "BSD";                         // BSD partition
        case 0xB4: return "QNX4.x";                      // QNX4.x partition
        case 0xB5: return "QNX4.x";                      // QNX4.x partition
        case 0xBF: return "Solaris";                    // Solaris partition
        case 0xC0: return "Cisco";                      // Cisco partition
        case 0xC1: return "Novell NetWare";             // Novell NetWare partition
        case 0xC6: return "Novell NetWare";             // Novell NetWare partition
        case 0xDA: return "Non-FS";                     // Non-file system partition
        case 0xDE: return "Dell Utility";               // Dell Utility partition
        case 0xE1: return "DOS 1.x";                    // DOS 1.x partition
        case 0xE3: return "DOS 2.x";                    // DOS 2.x partition
        case 0xE4: return "DOS 3.x";                    // DOS 3.x partition
        case 0xE5: return "DOS 4.x";                    // DOS 4.x partition
        case 0xE6: return "DOS 5.x";                    // DOS 5.x partition
        case 0xE7: return "DOS 6.x";                    // DOS 6.x partition
        case 0xEB: return "DOS 7.x";                    // DOS 7.x partition
        case 0xF0: return "DOS 8.x";                    // DOS 8.x partition
        case 0xF1: return "DOS 9.x";                    // DOS 9.x partition
        case 0xF2: return "NTFS";                       // NTFS file system
        case 0xF4: return "NTFS";                       // NTFS file system
        case 0xF8: return "BeOS";                       // BeOS partition
        case 0xFB: return "Bootable";                   // Bootable partition
        case 0xFE: return "EFI GPT";                    // EFI GPT partition
        default: return "Unknown";                      // Unknown or unrecognized partition type
    }
}





/**
 * Convert a sector count to megabytes.
 *
 * This helper function calculates the size in megabytes from the sector count.
 *
 * @param sector_count: The number of sectors.
 *
 * @return: The size in megabytes.
 */
static uint32_t sectors_to_mb(uint32_t sector_count) 
{
    return (uint32_t)(((uint64_t)sector_count * SECTOR_SIZE) / (1024 * 1024));
}





/**
 * Determine if a partition entry represents an empty partition.
 *
 * This helper function checks if both the starting LBA and sector count are zero,
 * indicating that the partition is empty.
 *
 * @param entry: Pointer to the MBR partition entry.
 *
 * @return: True if the partition is empty, otherwise false.
 */
static bool is_partition_empty(const MBR_PartitionEntry *entry) 
{
    return (entry->lba == 0 && entry->sector_count == 0);
}





/**
 * Print formatted information about an MBR partition.
 *
 * This function prints details of a partition entry including its status, start and end LBA,
 * sector count, size in megabytes, and type. It skips empty partitions.
 *
 * @param device: The device name or identifier to be printed.
 * @param index: The index of the partition.
 * @param entry: Pointer to the MBR partition entry.
 * @param base_lba: The base LBA from which to calculate the absolute start and end LBA.
 */
void MBR_print_partition_info(const char *device, int index, MBR_PartitionEntry *entry, uint32_t base_lba) 
{
    // Check for null pointers in the function arguments
    if (entry == NULL || device == NULL) 
    {
        fprintf(stderr, "Invalid arguments: NULL pointer provided.\n");
        return; // Exit the function if arguments are invalid
    }

    // Check if the partition is empty; if so, skip processing
    if (is_partition_empty(entry)) 
    {
        // Empty partition; no need to print information
        return;
    }

    // Calculate the starting and ending LBA (Logical Block Addressing) of the partition
    uint32_t start_lba = base_lba + entry->lba;
    uint32_t end_lba = start_lba + entry->sector_count - 1;

    // Convert the number of sectors to megabytes for size representation
    uint32_t size_mb = sectors_to_mb(entry->sector_count);

    // Determine the boot indicator based on the status byte
    char boot_indicator = (entry->status == 0x80) ? '*' : ' ';

    // Retrieve a descriptive string for the partition type
    const char *partition_type_desc = MBR_get_partition_type(entry->partition_type);

    // Print the partition information in a formatted manner
    printf("%-20s%-6d %-6c %-10u %-10u %-10u %-10u %-6X %-6s\n",
           device,                        // Device name
           index,                         // Partition index
           boot_indicator,                // Boot indicator ('*' if active, ' ' otherwise)
           start_lba,                     // Starting LBA of the partition
           end_lba,                       // Ending LBA of the partition
           entry->sector_count,           // Number of sectors in the partition
           size_mb,                       // Size of the partition in MB
           entry->partition_type,         // Partition type code
           partition_type_desc);          // Description of the partition type
}





/**
 * Reads a sector from the device into a buffer.
 *
 * @param fd: File descriptor for the device.
 * @param lba: Logical Block Address to read from.
 * @param buf: Buffer to store the read data.
 * @return 0 on success, -1 on failure.
 */
static int read_sector(int fd, uint32_t lba, char *buf) 
{
    if (lseek(fd, lba * SECTOR_SIZE, SEEK_SET) == (off_t)-1) 
    {
        return -1; // Error seeking to the sector
    }

    if (read(fd, buf, SECTOR_SIZE) != SECTOR_SIZE) 
    {
        return -1; // Error reading the sector
    }

    return 0; // Success
}





/**
 * Prints the partition information from the provided entry.
 *
 * @param device: Device name for printing information.
 * @param index: Pointer to the partition index counter.
 * @param entry: Pointer to the partition entry to print.
 * @param base_lba: Base LBA for calculating partition addresses.
 */
static void print_partition_info(const char *device, int *index, MBR_PartitionEntry *entry, uint32_t base_lba) 
{
    // Skip empty partitions
    if (entry->lba == 0 && entry->sector_count == 0) 
    {
        return;
    }

    uint32_t start_lba = base_lba + entry->lba;
    uint32_t end_lba = start_lba + entry->sector_count - 1;
    double size_mb = (double)(entry->sector_count * SECTOR_SIZE) / (1024 * 1024);
    char boot_indicator = (entry->status == 0x80) ? '*' : ' ';
    const char *partition_type_desc = MBR_get_partition_type(entry->partition_type);

    printf("%-20s%-6d %-6c %-10u %-10u %-10u %-10f %-6X %-6s\n",
           device,                        // Device name
           (*index)++,                    // Increment and print the partition index
           boot_indicator,                // Boot indicator ('*' if active, ' ' otherwise)
           start_lba,                     // Starting LBA of the partition
           end_lba,                       // Ending LBA of the partition
           entry->sector_count,           // Number of sectors in the partition
           size_mb,                       // Size of the partition in MB
           entry->partition_type,         // Partition type code
           partition_type_desc);          // Description of the partition type
}





/**
 * Parses and prints Extended Boot Record (EBR) partitions.
 *
 * @param fd: File descriptor for the device.
 * @param device: Device name for printing information.
 * @param current_partition_lba: LBA of the current partition to start parsing.
 * @param index: Pointer to the partition index counter.
 */
void MBR_parse_ebr(int fd, const char *device, uint32_t current_partition_lba, int *index) 
{
    char buf[SECTOR_SIZE];
    MBR_PartitionEntry *entry;

    while (current_partition_lba != 0) 
    {
        // Read the sector from the device
        if (read_sector(fd, current_partition_lba, buf) != 0) 
        {
            fprintf(stderr, "Error reading sector at LBA %u\n", current_partition_lba);
            break;
        }

        // Skip the first 446 bytes and point to the partition entries
        entry = (MBR_PartitionEntry *)&buf[446];

        // Print the partition information
        print_partition_info(device, index, &entry[0], current_partition_lba);

        // Move to the next EBR if there is a valid partition
        if (entry[1].lba != 0) 
        {
            current_partition_lba += entry[1].lba;
        } 
        else 
        {
            // No more logical partitions
            break;  
        }
    }
}

