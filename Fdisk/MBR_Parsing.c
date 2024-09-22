/**
 *===================================================================================
 * @file           : MBR_Parsing.c
 * @author         : Ali Mamdouh
 * @brief          : source file to parse and print MBR  partition tables
 * @Reviewer       : Eng Reda
 * @Version        : 2.0.0
 *===================================================================================
 * 
 *===================================================================================
 */




/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include "MBR_Parsing.h"  // Includes the header file "MBR.h" which defines structures and functions related to Master Boot Record (MBR) partition handling





/*============================================================================
 ******************************  Functions  **********************************
 ============================================================================*/
/**
 * Convert sector count to megabytes.
 *
 * This helper function calculates the size in megabytes from the given sector count.
 *
 * @param sector_count: The number of sectors.
 *
 * @return The size in megabytes.
 */
static uint32_t convert_sectors_to_mb(uint32_t sector_count) 
{
    // Calculate and return size in MB
    return (uint32_t)((((uint64_t)sector_count * SECTOR_SIZE) + (1024 * 1024 - 1)) / (1024 * 1024));
}







/**
 * Check if a partition entry is empty.
 *
 * This helper function determines if a partition entry is empty based on its LBA and sector count.
 *
 * @param entry: Pointer to the MBR partition entry.
 *
 * @return 1 if the partition is empty, 0 otherwise.
 */
static int is_partition_empty(const MBR_PartitionEntry *entry) 
{
    return (entry->lba == 0 && entry->sector_count == 0);
}








/**
 * Get the description of an MBR partition type based on the type code.
 *
 * This function maps the given partition type code to a human-readable string
 * describing the type of partition. The type codes correspond to various filesystems,
 * utilities, and other partition types commonly used in MBR partition tables.
 *
 * @param type: The partition type code as an unsigned char.
 *
 * @return A string describing the partition type. If the type code is not recognized,
 *         "Unknown" is returned.
 */
const char* MBR_get_partition_type(unsigned char type) 
{
    switch (type) 
    {
        case 0x00: return "Empty";                        // Indicates an unused or unallocated partition
        case 0x01: return "FAT12";                       // FAT12 filesystem, used in older systems and floppy disks
        case 0x04: return "FAT16 (small)";               // FAT16 filesystem, for partitions smaller than 32 MB
        case 0x05: return "Extended";                    // Extended partition, used to create logical partitions within it
        case 0x06: return "FAT16";                       // FAT16 filesystem, for partitions larger than 32 MB
        case 0x07: return "NTFS or exFAT";               // NTFS (New Technology File System) or exFAT (Extended FAT)
        case 0x0B: return "FAT32 (CHS)";                 // FAT32 filesystem using Cylinder-Head-Sector addressing
        case 0x0C: return "W95 FAT32 (LBA)";                 // FAT32 filesystem using Logical Block Addressing
        case 0x0E: return "FAT16 (LBA)";                 // FAT16 filesystem using Logical Block Addressing
        case 0x11: return "Hidden FAT16";                // Hidden FAT16 filesystem, not visible in standard file explorers
        case 0x12: return "Compaq Diagnostics";          // Compaq Diagnostics partition, used for diagnostic tools
        case 0x14: return "FAT16 (large)";               // FAT16 filesystem, for larger partitions
        case 0x16: return "Hidden FAT16 (LBA)";          // Hidden FAT16 filesystem with LBA addressing
        case 0x1B: return "Hidden FAT32 (CHS)";          // Hidden FAT32 filesystem using CHS addressing
        case 0x1C: return "Hidden FAT32 (LBA)";          // Hidden FAT32 filesystem with LBA addressing
        case 0x1E: return "Hidden FAT16 (LBA)";          // Hidden FAT16 filesystem with LBA addressing
        case 0x20: return "Dynamic Disk";                // Dynamic disk partition, used in modern Windows systems
        case 0x27: return "Hidden NTFS/HPFS";            // Hidden NTFS or HPFS partition
        case 0x39: return "Plan 9";                      // Plan 9 partition type
        case 0x3C: return "PartitionMagic";              // PartitionMagic partition type
        case 0x80: return "Linux Swap";                  // Linux swap partition, used for swapping data
        case 0x81: return "Linux";                       // Linux filesystem partition
        case 0x82: return "Linux Swap / Solaris";        // Linux swap or Solaris partition
        case 0x83: return "Linux";                       // Linux filesystem partition
        case 0x84: return "OS/2 hidden";                 // OS/2 hidden partition
        case 0x85: return "Linux Extended";              // Linux extended partition for logical partitions
        case 0x86: return "NTFS volume set";             // NTFS volume set, used in Windows systems
        case 0x87: return "NTFS volume set (no formatting)"; // NTFS volume set without formatting
        case 0x8A: return "OS/2 Boot Manager";           // OS/2 Boot Manager partition
        case 0x8B: return "OS/2 Boot Manager";           // OS/2 Boot Manager partition
        case 0xA0: return "IBM ThinkPad";                // IBM ThinkPad partition type
        case 0xA5: return "FreeBSD/NetBSD";              // FreeBSD or NetBSD partition type
        case 0xA6: return "OpenBSD";                     // OpenBSD partition type
        case 0xA8: return "Mac OS X";                    // Mac OS X partition type
        case 0xB1: return "BSD";                         // BSD partition type
        case 0xB4: return "QNX4.x";                      // QNX4.x partition type
        case 0xB5: return "QNX4.x";                      // QNX4.x partition type
        case 0xBF: return "Solaris";                     // Solaris partition type
        case 0xC0: return "Cisco";                       // Cisco partition type
        case 0xC1: return "Novell NetWare";              // Novell NetWare partition type
        case 0xC6: return "Novell NetWare";              // Novell NetWare partition type
        case 0xDA: return "Non-FS";                      // Non-filesystem partition type
        case 0xDE: return "Dell Utility";                // Dell Utility partition type
        case 0xE1: return "DOS 1.x";                     // DOS 1.x partition type
        case 0xE3: return "DOS 2.x";                     // DOS 2.x partition type
        case 0xE4: return "DOS 3.x";                     // DOS 3.x partition type
        case 0xE5: return "DOS 4.x";                     // DOS 4.x partition type
        case 0xE6: return "DOS 5.x";                     // DOS 5.x partition type
        case 0xE7: return "DOS 6.x";                     // DOS 6.x partition type
        case 0xEB: return "DOS 7.x";                     // DOS 7.x partition type
        case 0xF0: return "DOS 8.x";                     // DOS 8.x partition type
        case 0xF1: return "DOS 9.x";                     // DOS 9.x partition type
        case 0xF2: return "NTFS";                        // NTFS filesystem partition
        case 0xF4: return "NTFS";                        // NTFS filesystem partition
        case 0xF8: return "BeOS";                        // BeOS partition type
        case 0xFB: return "Bootable";                    // Bootable partition
        case 0xFE: return "EFI GPT";                     // EFI GPT partition type
        default: return "Unknown";                       // Default case for unrecognized partition types
    }
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
    // Check for invalid arguments
    if (device == NULL || entry == NULL) 
    {
        fprintf(stderr, "Invalid arguments: NULL pointer provided.\n");
        return;
    }

    // Check if the partition is empty; if so, skip processing
    if (is_partition_empty(entry)) 
    {
        return;
    }

    // Calculate the starting and ending LBA of the partition
    uint32_t start_lba = base_lba + entry->lba;
    uint32_t end_lba = start_lba + entry->sector_count - 1;

    // Determine the boot indicator based on the status byte
    char boot_indicator = (entry->status == 0x80) ? '*' : ' ';

    // Retrieve a descriptive string for the partition type
    const char *partition_type_desc = MBR_get_partition_type(entry->partition_type);

    // Calculate the size in megabytes
    uint32_t size_mb = convert_sectors_to_mb(entry->sector_count);

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
 * Reads a sector from the disk at a specific LBA.
 *
 * This helper function reads a single sector from the disk and stores it in the buffer.
 *
 * @param fd: File descriptor for the disk.
 * @param lba: Logical Block Address (LBA) to read from.
 * @param buf: Buffer to store the read data.
 *
 * @return 0 on success, -1 on failure.
 */
static int read_sector(int fd, uint32_t lba, char *buf) 
{
    if (lseek(fd, lba * SECTOR_SIZE, SEEK_SET) == -1) 
    {
        perror("lseek error");
        return -1;
    }

    if (read(fd, buf, SECTOR_SIZE) != SECTOR_SIZE) 
    {
        perror("read error");
        return -1;
    }

    return 0;
}






/**
 * Moves to the next logical partition entry in the EBR.
 *
 * This helper function checks the second entry in the EBR to determine if there is another
 * logical partition and returns its LBA if present.
 *
 * @param entry: Pointer to the first partition entry in the EBR.
 * @param first_ebr_lba: The LBA of the first EBR to calculate the absolute LBA of the next logical partition.
 *
 * @return The LBA of the next logical partition if it exists, or 0 if there are no more logical partitions.
 */
static uint32_t get_next_ebr_lba(const MBR_PartitionEntry *entry, uint32_t first_ebr_lba) 
{
    // The second partition entry in the EBR points to the next EBR (if any)
    if (entry[1].lba != 0) 
    {
        return first_ebr_lba + entry[1].lba;
    } 
    else 
    {
        return 0;  // No more logical partitions
    }
}







/**
 * Parse and print information from the Extended Boot Record (EBR).
 *
 * This function traverses through the Extended Boot Record (EBR) chain to parse and print
 * details of all logical partitions within the EBR. It starts from the first EBR (given by 
 * `first_ebr_lba`) and continues to the next linked EBR until no more logical partitions 
 * are found.
 *
 * It prints the information for each logical partition using the `MBR_print_partition_info` 
 * function and increments the partition index for each logical partition.
 *
 * @param fd: The file descriptor representing the disk device. The file must be opened
 *            and point to the appropriate disk (e.g., `/dev/sda`).
 * @param device: A string representing the device name or identifier (e.g., `/dev/sda`), 
 *                which is printed as part of the partition information.
 * @param first_ebr_lba: The LBA (Logical Block Addressing) of the first EBR in the chain. 
 *                       This LBA is used as the starting point to traverse the linked EBRs.
 * @param index: A pointer to an integer representing the partition index. This index will 
 *               be incremented with each logical partition found and printed. It is passed 
 *               by reference so the calling function can keep track of the current partition number.
 * 
 * 
 */
void MBR_parse_ebr(int fd, const char *device, uint32_t first_ebr_lba, int *index) 
{
    // Validate input arguments
    if (device == NULL || index == NULL) 
    {
        fprintf(stderr, "Invalid arguments: NULL pointer provided.\n");
        return;
    }

    char buf[SECTOR_SIZE];  // Buffer to store the sector data
    MBR_PartitionEntry *entry;  // Pointer to the partition entry structure

    uint32_t current_partition_lba = first_ebr_lba;  // Start with the first EBR

    // Loop through each linked EBR until there are no more logical partitions
    while (current_partition_lba != 0) 
    {
        // Read the current EBR sector into the buffer
        if (read_sector(fd, current_partition_lba, buf) == -1) 
        {
            fprintf(stderr, "Error reading EBR at LBA: %u\n", current_partition_lba);
            return;
        }

        // Skip the first 446 bytes (boot code) to get to the partition entries
        entry = (MBR_PartitionEntry *)&buf[446];

        // Print the partition information using the helper function
        MBR_print_partition_info(device, *index, entry, current_partition_lba);

        // Increment the partition index
        (*index)++;  

        // Move to the next linked EBR (if any)
        current_partition_lba = get_next_ebr_lba(entry, first_ebr_lba);
    }
}
