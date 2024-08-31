/**
 *===================================================================================
 * @file           : GPT_Parsing.c
 * @author         : Ali Mamdouh
 * @brief          : source of GPT_Parsing
 * @Reviewer       : Eng Reda
 * @Version        : 1.0.0
 *===================================================================================
 * 
 *===================================================================================
 */





/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include "GPT_Parsing.h"





/*============================================================================
 *************************  Variable Initialization  *************************
 ============================================================================*/ 
GPT_PartitionType partition_types[] = 
{
    {"EBD0A0A2-B9E5-4433-87C0-68B6B72699C7", "Microsoft basic data"},               // Microsoft basic data
    {"C12A7328-F81F-11D2-BA4B-00A0C93EC93B", "EFI System"},                         // EFI System
    {"E3C9E316-0B5C-4DB8-817D-F92DF00215AE", "Microsoft reserved"},                 // Microsoft reserved
    {"DE94BBA4-06D1-4D40-A16A-BFD50179D6AC", "Windows recovery environment"},       // Windows recovery environment
    {"0FC63DAF-8483-4772-8E79-3D69D8477DE4", "Linux filesystem"},                   // Linux filesystem
    {"A19D880F-05FC-4D3B-A006-743F0F84911E", "Linux swap"},                        // Linux swap
    {"48465300-0000-11AA-AA11-00306543ECAC", "Apple HFS+"},                        // Apple HFS+
    {"426F6F74-0000-11AA-AA11-00306543ECAC", "Apple Boot"},                         // Apple Boot
    {"21686148-6449-48D9-BC8D-4D8FBD6D1B3F", "BIOS Boot"},                         // BIOS Boot
    {"C6E9E2E0-8D14-11E8-B83E-1C1B7F13E7F8", "Linux RAID"},                       // Linux RAID
    {"D9E5C4A2-8F9F-4F16-93F8-228EF7E6D0F3", "Linux LVM"},                        // Linux LVM
    {"6A30D3D8-B6A2-4D23-B0F7-FD9B47F64E3A", "Windows Data Partition"},            // Windows Data Partition
    {"7C3457A4-8C28-4F9D-8358-0F5C2B3E2CF2", "Windows Recovery Partition"},        // Windows Recovery Partition
    {"F4E1D80E-FDF3-4F12-90CE-F3F97924E1A0", "HP-UX Data"},                       // HP-UX Data
    {"3B8F8E50-3E3A-4F1F-9A77-5B58D4D6F5A6", "Solaris Boot"},                     // Solaris Boot
    {"EBD0A0A2-B9E5-4433-87C0-68B6B72699C7", "Microsoft basic data"},               // Duplicate entry for clarity
    {NULL, NULL}  // End of array marker
};




/*============================================================================
 ******************************  Functions  **********************************
 ============================================================================*/
/**
 * Formats a byte as a two-digit hexadecimal string.
 *
 * This helper function converts a single byte into its corresponding
 * two-digit hexadecimal representation and stores the result in the
 * provided buffer. The buffer should be large enough to hold at least
 * two characters plus a null terminator.
 *
 * @param byte: The byte value to be formatted. This should be an 8-bit
 *              unsigned integer (0-255).
 * @param buffer: A pointer to a buffer where the formatted hexadecimal
 *                string will be stored. The buffer should be at least
 *                3 bytes long to accommodate the two-digit hex value and
 *                the null terminator.
 *
 * @note The buffer is expected to be properly allocated and have enough
 *       space to store the formatted string. If the buffer is too small,
 *       it may lead to buffer overflow and undefined behavior.
 */
static void format_byte_as_hex(uint8_t byte, char *buffer) 
{
    sprintf(buffer, "%02X", byte);
}



/**
 * Converts a GUID to its string representation in standard UUID format.
 *
 * This function takes a GUID (Globally Unique Identifier) represented as
 * an array of 16 bytes and converts it into a string formatted as a UUID
 * (Universally Unique Identifier) in the canonical 8-4-4-4-12 hexadecimal
 * format. The resulting string is stored in the provided buffer.
 *
 * The GUID is formatted according to the following structure:
 *  - First 4 bytes: Little-endian order
 *  - Next 2 bytes: Little-endian order
 *  - Next 2 bytes: Little-endian order
 *  - Next 2 bytes: Big-endian order (no change)
 *  - Last 6 bytes: Big-endian order (no change)
 *
 * The output string is 36 characters long plus a null terminator (total 37 bytes).
 *
 * @param guid: A pointer to an array of 16 bytes representing the GUID.
 *              The array must be properly initialized and not NULL.
 * @param guid_str: A pointer to a buffer where the formatted GUID string
 *                  will be stored. The buffer should be at least 37 bytes
 *                  in size to accommodate the formatted string and null terminator.
 *
 * @note The function performs basic NULL pointer checks. If either parameter
 *       is NULL, an error message is printed to stderr, and the function returns
 *       without modifying the buffer. Ensure that the provided buffer is
 *       sufficiently large to prevent buffer overflow.
 */
void convert_guid_to_string(const unsigned char *guid, char *guid_str) 
{
    if (guid == NULL || guid_str == NULL) 
    {
        fprintf(stderr, "Error: NULL pointer provided to convert_guid_to_string.\n");
        return;
    }

    // Format the GUID into the string representation
    snprintf(guid_str, 37,  // GUID string is 36 characters + null terminator
             "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
             // Little-endian order for first 8 bytes
             guid[3], guid[2], guid[1], guid[0], // First 4 bytes
             guid[5], guid[4],                   // Next 2 bytes
             guid[7], guid[6],                   // Next 2 bytes
             guid[8], guid[9],                   // Next 2 bytes
             guid[10], guid[11], guid[12], guid[13], guid[14], guid[15]); // Last 6 bytes
}




/**
 * Checks if a GUID prefix matches a known partition type.
 *
 * @param guid_prefix: The GUID prefix to match.
 * @param partition_type: The partition type structure to check against.
 * @return 1 if the prefix matches, otherwise 0.
 */
static int is_guid_prefix_match(const char *guid_prefix, const GPT_PartitionType *partition_type) 
{
    return strncmp(partition_type->guid_prefix, guid_prefix, strlen(partition_type->guid_prefix)) == 0;
}




/**
 * Maps the obtained GUID to the suitable GPT partition type.
 *
 * This function compares the given GUID prefix with known partition types
 * and returns a human-readable name for the matched type. If no match is found,
 * it returns "Unknown Type".
 *
 * @param type_guid: The GUID prefix to be matched.
 * @return A string representing the partition type name, or "Unknown Type" if no match is found.
 */
const char* GPT_get_partition_type(const char *type_guid) 
{
    if (type_guid == NULL) 
    {
        return "Invalid GUID";  // Handle NULL pointer for type_guid
    }

    for (int i = 0; partition_types[i].guid_prefix != NULL; i++) 
    {
        if (is_guid_prefix_match(type_guid, &partition_types[i])) 
        {
            return partition_types[i].name;
        }
    }

    return "Unknown Type";  
}




/**
 * Checks if the partition entry represents an empty partition.
 * 
 * @param entry: Pointer to the GPT partition entry.
 * @return 1 if the partition is empty, 0 otherwise.
 */
static int is_partition_empty(const GPT_PartitionEntry *entry) 
{
    return (entry->starting_lba == 0 && entry->ending_lba == 0);
}

/**
 * Formats the partition size in sectors to megabytes.
 * 
 * @param start_lba: Starting LBA of the partition.
 * @param end_lba: Ending LBA of the partition.
 * @return Size of the partition in megabytes.
 */
static uint64_t format_partition_size_in_mb(uint64_t start_lba, uint64_t end_lba) 
{
    uint64_t sector_count = end_lba - start_lba + 1;
    return (sector_count * SECTOR_SIZE) / (1024 * 1024);
}

/**
 * Prints the information of a GPT partition entry.
 *
 * @param device: The name of the device (e.g., "/dev/sda").
 * @param index: The partition index.
 * @param entry: Pointer to the GPT partition entry.
 */
void GPT_print_partition_info(const char *device, int index, GPT_PartitionEntry *entry) 
{
    if (entry == NULL || device == NULL) 
    {
        fprintf(stderr, "Error: NULL pointer provided to GPT_print_partition_info.\n");
        return;
    }

    if (is_partition_empty(entry)) 
    {
        // Skip empty partitions
        return;
    }

    // Convert the binary GUID to a string
    char guid_str[ GPT_TYPE_STRING_LENGTH + 1 ];  
    convert_guid_to_string(entry->type_guid, guid_str);

    // Map the GUID string to the partition type name
    const char *partition_type = GPT_get_partition_type(guid_str);

    // Calculate partition size in MB
    uint64_t partition_size_mb = format_partition_size_in_mb(entry->starting_lba, entry->ending_lba);

    // Print partition information
    printf("%-16s%-6d %-10llu %-10llu %-10llu %-10llu %-36s\n",
           device,
           index,
           (unsigned long long)entry->starting_lba,
           (unsigned long long)entry->ending_lba,
           (unsigned long long)(entry->ending_lba - entry->starting_lba + 1),
           (unsigned long long)partition_size_mb,
           partition_type
    );
}