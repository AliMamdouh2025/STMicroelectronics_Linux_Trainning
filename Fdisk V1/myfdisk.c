 /*===================================================================================
 * @file           : main.c
 * @author         : Ali Mamdouh
 * @brief          : main file
 * @Reviewer       : Eng Reda
 * @Version        : 2.0.0
 *===================================================================================
 * 
 *===================================================================================
 */





/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include "MBR_Parsing.h" // Includes the custom header file for MBR parsing functionalities (e.g., data structures, function declarations for handling MBR)
#include "GPT_Parsing.h" // Includes the custom header file for GPT parsing functionalities (e.g., data structures, function declarations for handling GPT)
#include <sys/types.h>   // Defines data types used in system calls (e.g., ssize_t, off_t)








/*============================================================================
 ******************************  Functions  **********************************
 ============================================================================*/
/**
 * Initializes the device file and reads the first sector into a buffer.
 *
 * This function performs the following tasks:
 * 1. Opens the specified device file for reading.
 * 2. Checks if the file descriptor is valid. If not, prints an error message and returns an error code.
 * 3. Reads the specified amount of data from the device file into the provided buffer.
 * 4. Verifies that the read operation was successful. If not, prints an error message, closes the file descriptor, and returns an error code.
 * 5. Returns the file descriptor if the operations are successful.
 *
 * @param device The path to the device file to be opened.
 * @param buf Pointer to a buffer where the read data will be stored.
 * @param buf_size The size of the buffer to read from the device file.
 * 
 * @return The file descriptor of the opened device file on success, or -1 if an error occurs.
 */
int initialize_device(const char *device, char *buf, size_t buf_size) 
{
    // Open the device file for reading
    int fd = open(device, O_RDONLY);
    if (fd < 0) 
    {
        // Print error message if the file cannot be opened
        perror("Failed to open device");
        return -1; // Return an error code indicating failure
    }

    // Read the specified amount of data from the device file into the buffer
    if (read(fd, buf, buf_size) != buf_size) 
    {
        // Print error message if the read operation fails
        perror("Failed to read sector");
        close(fd); // Close the file descriptor
        return -1; // Return an error code indicating failure
    }

    // Return the file descriptor if the operations are successful
    return fd;
}






/**
 * Prints the header information for GPT (GUID Partition Table) partitions.
 *
 * This function outputs a formatted header line to the standard output (console) for displaying 
 * GPT partition table information. The header includes the following columns:
 * 1. Device: The name or path of the device.
 * 2. Index: The partition index.
 * 3. Start: The starting Logical Block Address (LBA) of the partition(with Sector units).
 * 4. End: The ending LBA of the partition(with Sector units).
 * 5. Sectors: The total number of sectors in the partition.
 * 6. Size(MB): The size of the partition in megabytes.
 * 7. Type: The type of the partition, as identified by the GUID.
 *
 * The header is printed in a tabular format, with each column having a fixed width for alignment.
 */
void print_gpt_header_info() 
{
    // Print the GPT header information with fixed column widths
    printf("%-16s%-6s %-10s %-10s %-10s %-10s %-36s\n",
           "Device", "Index", "Start", "End", "Sectors", "Size(MB)", "Type");
}






/**
 * Reads the GPT (GUID Partition Table) header from a device.
 *
 * This function seeks to the location of the GPT header on the device and reads
 * the GPT header into the provided buffer. The function performs the following
 * operations:
 * 1. Seeks to the GPT header location using `lseek`.
 * 2. Reads the GPT header data from the device into the buffer using `read`.
 *
 * If any operation fails, an error message is printed, and the function returns
 * a non-zero error code. On success, the function returns 0.
 *
 * @param fd: The file descriptor of the device from which to read the GPT header.
 * @param buf: Pointer to a buffer where the GPT header data will be stored. 
 *             The buffer must be at least `SECTOR_SIZE` bytes in size.
 *
 * @return 0 on success, or -1 if an error occurs during seeking or reading.
 */
int read_gpt_header(int fd, char *buf) 
{
    // Seek to the GPT header location (LBA 1) on the device
    if (lseek(fd, GPT_HEADER_LBA * SECTOR_SIZE, SEEK_SET) == (off_t)-1) // Casting with same return type of lseek
    {
        perror("Failed to seek to GPT header");
        return -1; // Return an error code indicating failure to seek
    }

    // Read the GPT header data into the buffer
    if (read(fd, buf, SECTOR_SIZE) != SECTOR_SIZE) 
    {
        perror("Failed to read GPT header");
        return -1; // Return an error code indicating failure to read
    }

    return 0; // Return success code if seeking and reading were successful
}








/**
 * Reads GPT (GUID Partition Table) partition entries from a device and prints them.
 *
 * This function iterates through all GPT partition entries, reads each entry
 * from the device, and prints the partition information using the `GPT_print_partition_info` function.
 * The function performs the following operations:
 * 1. Iterates through the number of GPT entries defined by `GPT_ENTRIES_NUM`.
 * 2. Seeks to the location of each GPT entry using `lseek`.
 * 3. Reads the GPT entry data from the device into a local `GPT_PartitionEntry` structure.
 * 4. Prints the partition information.
 *
 * If any operation fails, an error message is printed, and the function stops processing further entries.
 *
 * @param fd: The file descriptor of the device from which to read the GPT partition entries.
 * @param device: The name of the device, used for printing the partition information.
 *
 * @return void
 */
void read_and_print_gpt_entries(int fd, const char *device) 
{
    GPT_PartitionEntry entry;
    int partition_index = 1;

    // Iterate through all GPT partition entries
    for (int i = 0; i < GPT_ENTRIES_NUM; i++) 
    {
        // Calculate the offset for the current GPT entry
        off_t offset = (u_int64_t)(GPT_ENTRY_ARRAY_LBA * SECTOR_SIZE) + (u_int64_t)(i * GPT_ENTRY_SIZE);
        
        // Seek to the GPT entry location
        if (lseek(fd, offset, SEEK_SET) == (off_t)-1) 
        {
            perror("Failed to seek to GPT entry");
            return; // Stop processing if seeking fails
        }

        // Read the GPT entry data into the entry structure
        if (read(fd, &entry, GPT_ENTRY_SIZE) != GPT_ENTRY_SIZE) 
        {
            perror("Failed to read GPT entry");
            return; // Stop processing if reading fails
        }

        // Print the partition information for the current entry
        GPT_print_partition_info(device, partition_index++, &entry);
    }
}





/**
 * Prints the header information for MBR (Master Boot Record) partition table.
 *
 * This function outputs column headers to the standard output, which describe the layout of MBR partition table entries.
 * The headers are formatted to align with the details that will be printed for each partition entry.
 *
 * The columns included in the header are:
 * - Device: The device name.
 * - Index: The index or number of the partition.
 * - Boot: Boot flag indicating if the partition is bootable.
 * - Start: The starting sector of the partition.
 * - End: The ending sector of the partition.
 * - Sectors: The total number of sectors in the partition.
 * - Size(MB): The size of the partition in megabytes.
 * - Id: The partition ID or type code.
 * - Type: The partition type as a descriptive string.
 *
 *
 * @return void
 */
void print_mbr_header_info() 
{
    printf("%-20s%-6s %-6s %-10s %-10s %-10s %-10s %-6s %-6s\n",
           "Device", "Index", "Boot", "Start", "End", "Sectors", "Size(MB)", "Id", "Type");
}





/**
 * Reads the MBR partition entries from a buffer and sets a pointer to the partition entries.
 *
 * This function extracts the MBR partition entries from the provided buffer and sets the `table_entry_ptr` 
 * to point to the start of the partition entries within the buffer.
 * 
 *
 * @param buf Pointer to the buffer containing the MBR data.
 * @param table_entry_ptr Pointer to a pointer where the address of the partition entries will be stored.
 * 
 * @return int Returns 0 on success. No errors are expected in this function.
 */
int read_mbr_partition_entries(const char *buf, MBR_PartitionEntry **table_entry_ptr) 
{
    *table_entry_ptr = (MBR_PartitionEntry *)&buf[446];
    return 0; // Success
}







/**
 * Processes and prints information about MBR partitions and handles extended partitions.
 *
 * This function iterates through the MBR partition entries, prints information for each valid partition, 
 * and processes any extended partitions to handle logical partitions within them.
 * 
 * The function starts by printing the MBR partition information, then checks for extended partitions. 
 * If an extended partition is detected, it parses and prints logical partitions contained within that extended partition.
 *
 * @param fd File descriptor for the device. Used for seeking and reading extended boot records (EBRs).
 * @param device Name of the device to be printed in the partition information.
 * @param table_entry_ptr Pointer to the array of MBR partition entries.
 * 
 * @return void This function does not return a value. It prints partition information and handles extended partitions.
 */
void process_mbr_partitions(int fd, const char *device, MBR_PartitionEntry *table_entry_ptr) 
{
    int partition_index = 1;
    int logical_partition_index = 5; // Start logical partitions at index 5

    for (int i = 0; i < MBR_PARTITIONS_NUM; i++) 
    {
        if (table_entry_ptr[i].lba != 0) 
        {
            MBR_print_partition_info(device, partition_index++, &table_entry_ptr[i], 0);

            // Check if this is an extended partition
            if (table_entry_ptr[i].partition_type == CHS_EXTENDED_PARTITION || 
                table_entry_ptr[i].partition_type == LBA_EXTENDED_PARTITION || 
                table_entry_ptr[i].partition_type == LINUX_EXTENDED_PARTITION) 
            { 
                MBR_parse_ebr(fd, device, table_entry_ptr[i].lba, &logical_partition_index);
            }
        }
    }
}







/*============================================================================
 ******************************  Main Code  **********************************
 ============================================================================*/
/**
 * Main function for partition table analysis.
 *
 * This function performs the following tasks:
 * 1. Validates command-line arguments to ensure the correct usage.
 * 2. Opens the specified device file for reading.
 * 3. Reads the first sector of the device to determine the partition table type.
 * 4. Based on the partition table type (GPT or MBR), performs the following:
 *    - For GPT (GUID Partition Table):
 *      - Prints header information for GPT partition entries.
 *      - Reads the GPT header and verifies its integrity.
 *      - Reads and prints GPT partition entries.
 *    - For MBR (Master Boot Record):
 *      - Prints header information for MBR partition entries.
 *      - Reads MBR partition entries.
 *      - Processes and prints details of MBR partitions, including handling extended partitions.
 * 5. Closes the device file and exits the program.
 *
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * 
 * @return 0 if the program executes successfully, 1 if an error occurs.
 */
int main(int argc, char **argv) 
{
    /* Open the device specified by the user */
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <device>\n", argv[0]);
        return 1;
    }

    char buf[SECTOR_SIZE];
    int fd = initialize_device(argv[1], buf, SECTOR_SIZE);
    if (fd < 0) 
    {
        return 1; // Error occurred during device initialization
    }

    /* Check if the device contains a GPT partition table */
    if ((uint8_t)buf[450] == GPT_SIGNATURE) 
    {
        /* Print the header information for GPT partitions */
        print_gpt_header_info();

        /* Read the GPT header and handle any errors */
        if (read_gpt_header(fd, buf) != 0) 
        {
            close(fd);
            return 1; // Error reading GPT header
        }

        /* Read and print GPT partition entries */
        read_and_print_gpt_entries(fd, argv[1]);
    }

    /* Check if the device contains an MBR partition table */
    else if (*(uint16_t*)&buf[510] == MBR_SIGNATURE)
    {
        /* Print the header information for MBR partitions */
        print_mbr_header_info();

        /* Read the MBR partition entries */
        MBR_PartitionEntry *table_entry_ptr;
        if (read_mbr_partition_entries(buf, &table_entry_ptr) != 0) 
        {
            close(fd);
            return 1; // Error reading MBR partition entries
        }

        /* Process and print MBR partition details */
        process_mbr_partitions(fd, argv[1], table_entry_ptr);
    }

    /* Close the device file descriptor */
    close(fd);
    return 0;
}
