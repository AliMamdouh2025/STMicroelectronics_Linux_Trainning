/**
 *===================================================================================
 * @file           : nyfdisk.c
 * @author         : Ali Mamdouh
 * @brief          : simulate fdisk command to show partition tables
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
 *===================================================================================
 * 
 *===================================================================================
 */






/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "MBR_Parsing.h"
#include "GPT_Parsing.h"






/*============================================================================
 *************************  Function prototypes  *****************************
 ============================================================================*/
void print_usage(const char *prog_name);
int open_device(const char *device);
void process_gpt(int fd, const char *device);
void process_mbr(int fd, const char *device);







/*============================================================================
 ******************************  Main Code  **********************************
 ============================================================================*/
/**
 * Main function to handle disk partition information based on the provided device.
 *
 * This function performs the following tasks:
 * 1. Checks if the correct number of command-line arguments are provided.
 * 2. Opens the specified device file in read-only mode.
 * 3. Reads the first sector of the device to determine the partition scheme (MBR or GPT).
 * 4. Depending on the partition scheme, it processes the partition table and prints partition information.
 * 5. Closes the device file before exiting.
 *
 * @param argc: Number of command-line arguments passed to the program.
 *              It should be exactly 2 (program name and device path).
 * @param argv: Array of command-line arguments.
 *              argv[1] should be the path to the device file to be analyzed.
 *
 * @return: Returns 0 on success, 1 on failure.
 */
int main(int argc, char **argv) 
{
    // Ensure exactly one argument (device path) is provided.
    if (argc != 2) 
    {
        print_usage(argv[0]);
        return 1;
    }

    // Open the device file in read-only mode.
    int fd = open_device(argv[1]);
    if (fd < 0) 
    {
        perror("Failed to open device");
        return 1;
    }

    char buf[SECTOR_SIZE];
    
    // Read the first sector of the device to check the partition scheme.
    ssize_t bytes_read = read(fd, buf, SECTOR_SIZE);
    if (bytes_read != SECTOR_SIZE) 
    {
        perror("Failed to read sector");
        close(fd);
        return 1;
    }

    // Check for GPT protective MBR signature at offset 450.
    if ((uint8_t)buf[450] == 0xEE) 
    {
        // Process and print GPT partition information.
        process_gpt(fd, argv[1]);
    } 
    else if(*(uint16_t*)&buf[510] == 0xAA55) // Little endian device so it has 0x55 in least byte
    {
        // Process and print MBR partition information.
        process_mbr(fd, argv[1]);
    }

    // Close the device file.
    close(fd);
    return 0;
}





/**
 * Print the usage message for the program.
 *
 * This function displays a message on the standard error stream to inform the user
 * about the correct way to run the program.
 *
 *
 * @param prog_name:  Pointer to a string representing the name of the program.
 *                    This is typically `argv[0]` from the `main` function. 
 *                    It is used in the usage message to show how the user should 
 *                    invoke the program.
 *
 */
void print_usage(const char *prog_name) 
{
    fprintf(stderr, "Usage: %s <device>\n", prog_name);
}





/**
 * Open a device file for reading.
 *
 * This function attempts to open the specified device file in read-only mode. 
 * It uses the 'open' system call with the `O_RDONLY` flag to ensure that the file 
 * is opened in read-only mode, preventing any modifications to the file.
 *
 * The function returns a file descriptor that can be used for subsequent read 
 * operations on the device file. If the file cannot be opened, the function returns 
 * -1, and the `errno` variable is set to indicate the error.
 *
 * @param device: Pointer to a string representing the path to the device file.
 *                This is the file path to the device that the program intends to 
 *                read from. The device file must be accessible and readable.
 *
 * @return: On success, returns a non-negative integer representing the file descriptor.
 *          On failure, returns -1 and sets 'errno' to indicate the error.
 *
 */
int open_device(const char *device) 
{
    return open(device, O_RDONLY);
}




/**
 * Process and display partition information from a GPT (GUID Partition Table) scheme.
 *
 * This function reads and processes the GPT partition table entries from the specified device.
 * It first skips the protective MBR and reads the GPT header located at LBA 1. Then, it reads
 * each partition entry from the GPT entry array and prints the partition details.
 *
 * The function expects the device file descriptor to be valid and open in read-only mode.
 * It prints a header for the partition information followed by details of each partition.
 *
 * @param fd: File descriptor of the open device file. This must be a valid file descriptor for
 *            the device from which the GPT data will be read.
 * @param device: Path to the device file. This string is used in the output to identify the
 *                 device being processed.
 */
void process_gpt(int fd, const char *device) 
{
    // Buffer to hold data read from the device
    char buf[SECTOR_SIZE];  

    // Print the header for the partition information output
    printf("%-16s%-6s %-10s %-10s %-10s %-10s %-36s\n",
           "Device", "Index", "Start", "End", "Sectors", "Size(MB)", "Type");

    // Skip the protective MBR and seek to the GPT header at LBA 1
    lseek(fd, GPT_HEADER_LBA * SECTOR_SIZE, SEEK_SET);

    // Read the GPT header into the buffer
    if (read(fd, buf, SECTOR_SIZE) != SECTOR_SIZE) 
    {
        perror("Failed to read GPT header");
        return;  // Exit the function if reading fails
    }

    // Initialize a GPT_PartitionEntry structure to hold each partition entry
    // Index to number partitions
    GPT_PartitionEntry entry;
    int partition_index = 1;  

    // Iterate through each partition entry in the GPT entry array
    for (int i = 0; i < GPT_ENTRIES_NUM; i++) 
    {
        // Seek to the location of the current partition entry
        lseek(fd, (GPT_ENTRY_ARRAY_LBA * SECTOR_SIZE) + (i * GPT_ENTRY_SIZE), SEEK_SET);

        // Read the current partition entry into the entry structure
        if (read(fd, &entry, GPT_ENTRY_SIZE) != GPT_ENTRY_SIZE) 
        {
            perror("Failed to read GPT partition entry");
            return;  // Exit the function if reading fails
        }

        // Print the details of the current partition entry
        GPT_print_partition_info(device, partition_index++, &entry);
    }
}





/**
 * Process and display partition information from an MBR (Master Boot Record) scheme.
 *
 * This function reads and processes the partition table entries from the MBR of the specified device.
 * It extracts partition entries from the MBR structure, skipping the initial 446 bytes used by the 
 * boot code and reads the partition table starting at byte offset 446. It then prints the details of
 * each partition and handles any extended partitions by parsing their entries.
 *
 * The function expects the device file descriptor to be valid and open in read-only mode.
 * It prints a header for the partition information followed by details of each partition, including
 * handling extended partitions by reading their respective entries.
 *
 * @param fd: File descriptor of the open device file. This must be a valid file descriptor for
 *            the device from which the MBR data will be read.
 * @param device: Path to the device file. This string is used in the output to identify the
 *                 device being processed.
 */
void process_mbr(int fd, const char *device) 
{
    // Buffer to hold data read from the device
    char buf[SECTOR_SIZE];  

    // Pointer to the partition table entries
    MBR_PartitionEntry *table_entry_ptr = (MBR_PartitionEntry *)&buf[446]; 

    // Print the header for the partition information output
    printf("%-20s%-6s %-6s %-10s %-10s %-10s %-10s %-6s %-6s\n", 
           "Device", "Index", "Boot", "Start", "End", "Sectors", "Size(MB)", "Id", "Type");

    int partition_index = 1;  // Index to number partitions
    int logical_partition_index = 5;  // Index to number logical partitions (starts at 5)

    // Iterate through the four primary partition entries in the MBR
    for (int i = 0; i < 4; i++) 
    {
        // Check if the partition entry is valid (lba != 0)
        if (table_entry_ptr[i].lba != 0) 
        {
            // Print the details of the current partition entry
            MBR_print_partition_info(device, partition_index++, &table_entry_ptr[i], 0);

            // Check if this partition entry is an extended partition type
            if (table_entry_ptr[i].partition_type == CHS_EXTENDED_PARTITION || 
                table_entry_ptr[i].partition_type == LBA_EXTENDED_PARTITION || 
                table_entry_ptr[i].partition_type == LINUX_EXTENDED_PARTITION) 
            { 
                // Process the extended boot record (EBR) for this extended partition
                MBR_parse_ebr(fd, device, table_entry_ptr[i].lba, &logical_partition_index);
            }
        }
    }
}

