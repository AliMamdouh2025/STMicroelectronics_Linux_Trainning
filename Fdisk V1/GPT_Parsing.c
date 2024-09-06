 /*===================================================================================
 * @file           : GPT_Parsing.c
 * @author         : Ali Mamdouh
 * @brief          : source of GPT_Parsing
 * @Reviewer       : Eng Reda
 * @Version        : 2.0.0
 *===================================================================================
 * 
 *===================================================================================
 */





/*============================================================================
 ******************************  Includes  ***********************************
 ============================================================================*/ 
#include "GPT_Parsing.h" // Includes the custom header file for GPT parsing functionalities (e.g., data structures, function declarations for handling GPT)






/*============================================================================
 ******************************  Functions  **********************************
 ============================================================================*/
/**
 * GPT Partition Type Array
 *
 * This array defines known GUID Partition Table (GPT) partition types by associating 
 * their GUID (Globally Unique Identifier) with a human-readable description. 
 * These partition types correspond to different filesystems, boot partitions, 
 * and other utilities commonly used in GPT-based partition tables.
 *
 * The GUIDs used in GPT are globally unique 128-bit numbers, and they define the
 * type of partition as well as the filesystem or operating system it belongs to.
 * 
 * The following table lists common GPT partition types:
 *
 * - Microsoft basic data: Used for Windows filesystems (e.g., NTFS, FAT32)
 * - EFI System: Contains the boot loader for UEFI systems
 * - Microsoft reserved: Reserved partition for Windows systems
 * - Windows recovery environment: Recovery partition for Windows
 * - Linux filesystem: Common partition for Linux filesystems (e.g., ext4)
 * - Linux swap: Used for swap space in Linux
 * - Apple HFS+: Filesystem used in macOS before APFS
 * - Apple Boot: Boot partition used by macOS systems
 * - VMware VMFS: Filesystem used in VMware environments
 * - Apple APFS: Apple's newer filesystem, used in macOS High Sierra and later
 * - BIOS boot partition: Partition used for BIOS booting on GPT systems
 * - MBR partition scheme: Protective MBR for backward compatibility
 * - Intel Fast Flash: Used by Intel for fast flash technologies
 * - Lenovo boot partition: Specific boot partition used by Lenovo devices
 *
 * @param guid: The partition type GUID as a string.
 * @param description: Human-readable description of the partition type.
 */
GPT_PartitionType partition_types[] = 
{
    {"EBD0A0A2-B9E5-4433-87C0-68B6B72699C7", "Microsoft basic data"},
    {"C12A7328-F81F-11D2-BA4B-00A0C93EC93B", "EFI System"},
    {"E3C9E316-0B5C-4DB8-817D-F92DF00215AE", "Microsoft reserved"},
    {"DE94BBA4-06D1-4D40-A16A-BFD50179D6AC", "Windows recovery environment"},
    {"0FC63DAF-8483-4772-8E79-3D69D8477DE4", "Linux filesystem"},
    {"A19D880F-05FC-4D3B-A006-743F0F84911E", "Linux swap"},
    {"48465300-0000-11AA-AA11-00306543ECAC", "Apple HFS+"},
    {"426F6F74-0000-11AA-AA11-00306543ECAC", "Apple Boot"},
    {"AA31E02A-400F-11DB-9590-000C2911D1B8", "VMware VMFS"},
    {"9D275380-40AD-11DB-BF97-000C2911D1B8", "VMware reserved"},
    {"7C3457EF-0000-11AA-AA11-00306543ECAC", "Apple APFS"},
    {"21686148-6449-6E6F-744E-656564454649", "BIOS boot partition"},
    {"024DEE41-33E7-11D3-9D69-0008C781F39F", "MBR partition scheme"},
    {"D3BFE2DE-3DAF-11DF-BA40-E3A556D89593", "Intel Fast Flash"},
    {"7412F7D5-A156-4B13-81DC-867174929325", "Lenovo boot partition"},
    {NULL, NULL}
};








/**
 * Converts a 16-byte binary GUID into its standard string representation.
 *
 * The GUID is provided in binary form, which needs to be interpreted in both
 * little-endian and big-endian formats as specified in the UUID standard. The
 * resulting string is formatted as XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.
 *
 * @param guid: Pointer to the 16-byte binary GUID.
 * @param guid_str: Buffer to store the resulting GUID string. Must be at least 37 characters long.
 * @return: Returns true if successful, false if inputs are invalid.
 */
bool convert_guid_to_string(const unsigned char *guid, char *guid_str) 
{
    // Validate input parameters
    if (guid == NULL || guid_str == NULL) 
    {
        fprintf(stderr, "Invalid input: guid and guid_str cannot be NULL\n");
        return false;
    }

    // Convert the GUID binary data to a string format (XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX)
    sprintf(guid_str,
            "%02X%02X%02X%02X-"    // First group (little-endian, 4 bytes)
            "%02X%02X-"            // Second group (little-endian, 2 bytes)
            "%02X%02X-"            // Third group (little-endian, 2 bytes)
            "%02X%02X-"            // Fourth group (big-endian, 2 bytes)
            "%02X%02X%02X%02X%02X%02X",  // Fifth group (big-endian, 6 bytes)
            guid[3], guid[2], guid[1], guid[0],   // Little-endian conversion
            guid[5], guid[4],                     // Little-endian conversion
            guid[7], guid[6],                     // Little-endian conversion
            guid[8], guid[9],                     // Big-endian, no conversion needed
            guid[10], guid[11], guid[12], guid[13], guid[14], guid[15]);  // Big-endian, no conversion needed

    return true;
}










/**
 * Maps a GUID string to the corresponding GPT partition type.
 *
 * This function takes a GUID string and searches the partition_types array for a match.
 * If the GUID matches a known type, it returns the corresponding partition name.
 * If no match is found, it returns "Unknown Type".
 *
 * @param type_guid: A string representing the GUID of the partition type.
 * @return: A string describing the partition type, or "Unknown Type" if no match is found.
 */
const char* GPT_get_partition_type(const char *type_guid) 
{
    // Validate the input parameter
    if (type_guid == NULL) 
    {
        fprintf(stderr, "Error: Invalid input - type_guid cannot be NULL\n");
        return "Invalid GUID";
    }

    // Ensure the input GUID string is of valid length
    if (strlen(type_guid) != GUID_LEN) 
    {
        fprintf(stderr, "Error: Invalid GUID length\n");
        return "Invalid GUID";
    }

    // Iterate through the known partition types and check for a match
    for (int i = 0; partition_types[i].guid_prefix != NULL; i++) 
    {
        // Compare the GUID prefixes
        if (strncmp(partition_types[i].guid_prefix, type_guid, GUID_LEN) == GUID_STRING_MATCH) 
        {
            return partition_types[i].name;  // Return the matching partition type
        }
    }

    // If no match is found, return "Unknown Type"
    return "Unknown Type";  
}







/**
 * Prints information about a GPT partition.
 *
 * This function prints detailed information about a GPT partition entry,
 * including its start and end LBA, size in megabytes, and partition type.
 * It skips empty partitions where both starting and ending LBA are zero.
 *
 * @param device: The name or identifier of the device where the partition resides.
 * @param index: The index of the partition.
 * @param entry: Pointer to the GPT partition entry structure containing partition details.
 */
void GPT_print_partition_info(const char *device, int index, GPT_PartitionEntry *entry) 
{
    // Validate input parameters
    if (device == NULL || entry == NULL) 
    {
        fprintf(stderr, "Error: Invalid input - device or entry cannot be NULL\n");
        return;
    }

    // Skip empty partitions
    if (entry->starting_lba == 0 && entry->ending_lba == 0) 
    {
        return;  // Empty partition; no information to print
    }

    // Convert GUID to string
    char guid_str[GUID_STR_LEN]; 
    convert_guid_to_string(entry->type_guid, guid_str);

    // Map GUID to partition type
    const char *partition_type = GPT_get_partition_type(guid_str);

    // Calculate size in megabytes
    uint64_t sector_count = entry->ending_lba - entry->starting_lba + 1;
    uint64_t size_mb = (sector_count * SECTOR_SIZE) / (1024 * 1024);

    // Print partition information
    printf("%-16s%-6d %-10llu %-10llu %-10llu %-10llu %-36s\n",
           device,                         // Device name
           index,                          // Partition index
           (unsigned long long)entry->starting_lba,  // Starting LBA
           (unsigned long long)entry->ending_lba,    // Ending LBA
           (unsigned long long)sector_count,          // Number of sectors
           (unsigned long long)size_mb,               // Size in megabytes
           partition_type                     // Partition type description
          );
}
