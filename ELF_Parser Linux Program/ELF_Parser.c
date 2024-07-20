/**
 ******************************************************************************
 * @file           : Elf_Parser.c
 * @author         : Ali Mamdouh
 * @brief          : Analyse and print Elf header and Elf secion informations
 ******************************************************************************
 * 
 ******************************************************************************
 */
 
 
 
 
 
/****************************************************************************
*******************************  Includes  **********************************
*****************************************************************************/
#include <stdio.h> // Used for file operations (fopen, fread, fprintf, etc.).
#include <stdlib.h> // Used for memory management and process control (malloc, free, exit).
#include <stdint.h> // Ensures the use of fixed-width integer types for portability.
#include <endian.h> // Handles byte order conversions when reading or writing binary data.
#include <elf.h> //Provides definitions for interpreting ELF file structures.
#include <errno.h> //Reports and handles errors that occur during system and library calls.
#include <string.h> //Manages C strings and memory blocks (strerror for error messages).
#include <unistd.h> // Provides access to POSIX functions like getopt for command-line option parsing.




/****************************************************************************
********************************  Macros  ***********************************
*****************************************************************************/
#define FALSE                                       0
#define TRUE                                        1
#define SUCCESS_OPERATION                           1
#define ARRAYS_ARE_EQUAL                            0
#define ALL_OPTIONS_ARE_PARSED                     -1
#define FAILED                                      0






/****************************************************************************
**************************  Functions Definitions  **************************
*****************************************************************************/

/**
 * Print the flags from the ELF header.
 *
 * The flags in the ELF header provide additional information about the file, such as its
 * execution characteristics or compatibility attributes. The `e_flags` field is a bitmask,
 * where each bit represents a different attribute or capability. This function prints the
 * value of `e_flags` in hexadecimal format for inspection.
 *
 * @param header: Pointer to the ELF header structure (Elf64_Ehdr). This structure contains
 *                the `e_flags` field, which is a bitmask encoding various flags related to
 *                the ELF file's attributes or behavior.
 */
void print_flags(Elf64_Ehdr *header) 
{
    printf("  Flags:                             0x%x\n", header->e_flags);
}






/**
 * Print dictionary of flags types and their convension names.
 */
void print_flag_descriptions() 
{
    printf("Key to Flags:\n");
    printf("  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n");
    printf("  L (link order), O (extra OS processing required), G (group), T (TLS),\n");
    printf("  C (compressed), x (unknown), o (OS specific), E (exclude),\n");
    printf("  D (mbind), l (large), p (processor specific)\n\n");
}






/**
 * Print the ELF file magic number.
 * The magic number is a sequence of bytes used to identify the file format as ELF.
 *
 * @param e_ident: Pointer to the ELF identification array.
 */
void print_magic(const unsigned char *e_ident) 
{
    printf("  Magic:   ");
	
	// A for loop that iterates through each byte of the ELF identification array. 
	// EI_NIDENT is a constant representing the number of bytes in the ELF identification array, including the magic number.
    // Typically, EI_NIDENT is defined as 16.
    for (int i = 0; i < EI_NIDENT; ++i) 
	{
        printf("%02x ", e_ident[i]);  // Print each byte of the magic number in hexadecimal format.
    }
    printf("\n");
}





/**
 * Print the class of the ELF file, indicating whether it is 32-bit or 64-bit.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_class(Elf64_Ehdr *header) 
{
    printf("  Class:                             ");
    switch (header->e_ident[EI_CLASS]) 
	{
        case ELFCLASS32: printf("ELF32\n"); break;  // 32-bit ELF format.
        case ELFCLASS64: printf("ELF64\n"); break;  // 64-bit ELF format.
        default: printf("Unknown\n"); break;       // Unknown class.
    }
}






/**
 * Print the data encoding of the ELF file, specifying whether it is little-endian or big-endian.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_dataEncoding(Elf64_Ehdr *header) 
{
    printf("  Data:                              ");
    switch (header->e_ident[EI_DATA]) 
	{
        case ELFDATA2LSB: printf("2's complement, little endian\n"); break;  // Little-endian encoding.
        case ELFDATA2MSB: printf("2's complement, big endian\n"); break;     // Big-endian encoding.
        default: printf("Unknown\n"); break;                                  // Unknown data encoding.
    }
}






/**
 * Print the version of the ELF header.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_version(Elf64_Ehdr *header) 
{
    printf("  Version:                           %d (current)\n", header->e_version);
}






/**
 * Print the OS/ABI identifier, indicating the target operating system and ABI.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_osabi(Elf64_Ehdr *header) 
{
    printf("  OS/ABI:                            ");
    switch (header->e_ident[EI_OSABI]) 
	{
        case ELFOSABI_SYSV: printf("UNIX - System V\n"); break;  // UNIX - System V ABI.
        default: printf("Unknown\n"); break;                     // Unknown ABI.
    }
}






/**
 * Print the ABI version from the ELF header.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_abi_version(Elf64_Ehdr *header) 
{
    printf("  ABI Version:                       %d\n", header->e_ident[EI_ABIVERSION]);
}






/**
 * Print the type of the ELF file, such as relocatable or executable.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_type(Elf64_Ehdr *header) 
{
    printf("  Type:                              ");
    switch (header->e_type) 
	{
        case ET_REL: printf("REL (Relocatable file)\n"); break;      // Relocatable file.
        case ET_EXEC: printf("EXEC (Executable file)\n"); break;     // Executable file.
        case ET_DYN: printf("DYN (Position-Independent Executable file)\n"); break; // Shared library or position-independent executable.
        case ET_CORE: printf("CORE (Core file)\n"); break;            // Core file.
        default: printf("Unknown\n"); break;                         // Unknown file type.
    }
}






/**
 * Print the machine architecture for which the ELF file is intended.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_machine(Elf64_Ehdr *header) 
{
    printf("  Machine:                           ");
    switch (header->e_machine) 
	{
        case EM_X86_64: printf("Advanced Micro Devices X86-64\n"); break; // x86-64 architecture.
        default: printf("Unknown\n"); break;                              // Unknown architecture.
    }
}






/**
 * Print the entry point address where execution begins in the ELF file.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_entry(Elf64_Ehdr *header) 
{
    printf("  Entry point address:               0x%lx\n", header->e_entry);
}






/**
 * Print the offset in bytes from the start of the file where the program headers begin.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_phoff(Elf64_Ehdr *header) 
{
    printf("  Start of program headers:          %lu (bytes into file)\n", header->e_phoff);
}







/**
 * Print the offset in bytes from the start of the file where the section headers begin.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_shoff(Elf64_Ehdr *header) 
{
    printf("  Start of section headers:          %lu (bytes into file)\n", header->e_shoff);
}







/**
 * The print_section_flags function is designed to interpret and print- 
 * the flags associated with an ELF section header in a human-readable format.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_section_flags(Elf64_Xword sh_flags) 
{
    char flags[12] = {'\0'};
    char index = 0;
    if (sh_flags & SHF_WRITE)            flags[index++] = 'W'; //This flag indicates that the section contains data that is writable during program execution.
    if (sh_flags & SHF_ALLOC)            flags[index++] = 'A'; //Set for sections that need to be loaded into memory, such as .text, .data, and .bss. Sections without this flag, like .comment or .note, are not loaded into memory.
    if (sh_flags & SHF_EXECINSTR)        flags[index++] = 'X'; //Set for sections containing code, such as the .text section, which includes the program's machine instructions.
    if (sh_flags & SHF_MERGE)            flags[index++] = 'M'; //This flag indicates that the section contains data that can be merged to eliminate redundancy, Commonly used with read-only string sections.
    if (sh_flags & SHF_STRINGS)          flags[index++] = 'S'; //This flag indicates that the section contains null-terminated strings, Set for sections that store string data such as .rodata.str1.1.
    if (sh_flags & SHF_INFO_LINK)        flags[index++] = 'I'; //Used when the section has a relationship with another section, typically in symbol tables where sh_info points to the section header index of the associated string table.
    if (sh_flags & SHF_LINK_ORDER)       flags[index++] = 'L'; //This flag indicates that the section has special ordering requirements, often related to link order.
    if (sh_flags & SHF_OS_NONCONFORMING) flags[index++] = 'O'; //Set for sections containing data specific to the operating system, which might not follow standard conventions.
    if (sh_flags & SHF_GROUP)            flags[index++] = 'G'; //This flag indicates that the section is part of a section group.
    if (sh_flags & SHF_TLS)              flags[index++] = 'T'; //Set if the section contains thread-local storage.
    if (sh_flags & SHF_COMPRESSED)       flags[index++] = 'C'; //Set if the section is compressed.
    printf("%s", flags);
}







/**
 * Print the size of the ELF header in bytes.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_ehsize(Elf64_Ehdr *header) 
{
    printf("  Size of this header:               %u (bytes)\n", header->e_ehsize);
}







/**
 * Print the size of each program header in bytes.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_phentsize(Elf64_Ehdr *header) 
{
    printf("  Size of program headers:           %u (bytes)\n", header->e_phentsize);
}







/**
 * Print the number of program headers in the ELF file.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_phnum(Elf64_Ehdr *header) 
{
    printf("  Number of program headers:         %u\n", header->e_phnum);
}






/**
 * Print the size of each section header in bytes.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_shentsize(Elf64_Ehdr *header) 
{
    printf("  Size of section headers:           %u (bytes)\n", header->e_shentsize);
}






/**
 * Print the number of section headers in the ELF file.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_shnum(Elf64_Ehdr *header) 
{
    printf("  Number of section headers:         %u\n", header->e_shnum);
}






/**
 * Print the index of the section header string table.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_shstrndx(Elf64_Ehdr *header) 
{
    printf("  Section header string table index: %u\n", header->e_shstrndx);
}






// Define section type names for easier interpretation
const char* elf_section_type_names[] = {
    "NULL", "PROGBITS", "SYMTAB", "STRTAB", "RELA", "HASH",
    "DYNAMIC", "NOTE", "NOBITS", "REL", "SHLIB", "DYNSYM"
};







/**
 * Print the complete ELF header information.
 * This function calls individual functions to display each field of the ELF header.
 *
 * @param header: Pointer to the ELF header structure.
 */
void print_elf_header(const Elf64_Ehdr *header) 
{
    if (header == NULL) 
	{
        fprintf(stderr, "Error: Null pointer passed to print_elf_header\n");
        return;
    }
    printf("ELF Header:\n");
    print_magic(header->e_ident);
    print_class((Elf64_Ehdr *)header);
    print_dataEncoding((Elf64_Ehdr *)header);
    print_version((Elf64_Ehdr *)header);
    print_osabi((Elf64_Ehdr *)header);
    print_abi_version((Elf64_Ehdr *)header);
    print_type((Elf64_Ehdr *)header);
    print_machine((Elf64_Ehdr *)header);
    print_entry((Elf64_Ehdr *)header);
    print_phoff((Elf64_Ehdr *)header);
    print_shoff((Elf64_Ehdr *)header);
    print_flags((Elf64_Ehdr *)header);
    print_ehsize((Elf64_Ehdr *)header);
    print_phentsize((Elf64_Ehdr *)header);
    print_phnum((Elf64_Ehdr *)header);
    print_shentsize((Elf64_Ehdr *)header);
    print_shnum((Elf64_Ehdr *)header);
    print_shstrndx((Elf64_Ehdr *)header);
}







/**
 * Print section headers from the ELF file.
 *
 * @param file: File pointer to the ELF file.
 * @param header: Pointer to the ELF header structure.
 */
void print_section_headers(FILE *file, const Elf64_Ehdr *header) 
{
	// If Section headers offset or entry number = 0, this means that ELF has no Section headers
    if (header->e_shoff == FALSE || header->e_shnum == FALSE) 
	{
        printf("This file has no section headers.\n");
        return;
    }

    // I use Dynamic memory allocation for portabillity of memory size allocated, 
	// as number of Section Entries differs from one ELF file to another
    Elf64_Shdr *section_headers = malloc(sizeof(Elf64_Shdr) * header->e_shnum);
    if (!section_headers) 
	{
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

	// fseek is used to move the file pointer to a specific location in the file. 
	// This is necessary when we need to read or write data from/to a particular part of the file that isn't at the current file pointer position.
	// header->e_shoff is the offset from the beginning of the file where the section headers start.
	// SEEK_SET constant is used with fseek to specify that the offset should be set relative to the beginning of the file. 
	// Other possible values are SEEK_CUR (relative to the current position) and SEEK_END (relative to the end of the file).
    if (fseek(file, header->e_shoff, SEEK_SET) != FAILED) 
	{
        fprintf(stderr, "Failed to seek to section headers\n");
        free(section_headers);
        return;
    }

    // Reads the section headers from the file into the allocated memory. If reading fails, an error message is printed, memory is freed, and the function exits.
    if (fread(section_headers, sizeof(Elf64_Shdr), header->e_shnum, file) != header->e_shnum) 
	{
        fprintf(stderr, "Failed to read section headers\n");
        free(section_headers);
        return;
    }

    // Read section header string table if present.
	// The section header string table holds the names of all the sections in the ELF file. Each string in this table corresponds to the name of a section header.
	// Each section header in the ELF file has a field (sh_name) that contains an offset into the section header string table-
	// This offset points to the beginning of the section name string within the string table.
    char *shstrtab = NULL;
    if (header->e_shstrndx != SHN_UNDEF) 
	{
        Elf64_Shdr *shstrtab_header = &section_headers[header->e_shstrndx];
        shstrtab = malloc(shstrtab_header->sh_size);
        if (!shstrtab) 
		{
            fprintf(stderr, "Memory allocation failed for section header string table\n");
            free(section_headers);
            return;
        }

        // Seek to and read the section header string table.
        if (fseek(file, shstrtab_header->sh_offset, SEEK_SET) != FAILED ||
            fread(shstrtab, 1, shstrtab_header->sh_size, file) != shstrtab_header->sh_size) {
            fprintf(stderr, "Failed to read section header string table\n");
            free(section_headers);
            free(shstrtab);
            return;
        }
    }

    printf("There are %d section headers, starting at offset 0x%lx:\n\n", header->e_shnum, header->e_shoff);
    printf("Section Headers:\n");
    printf("  [Nr] Name              Type             Address           Offset\n");
    printf("       Size              EntSize          Flags  Link  Info  Align\n");

    // Print each section header.
	// header->e_shnum contains the number of section headers.
    for (int i = 0; i < header->e_shnum; i++) 
    {
        // section_headers[i] accesses the i-th section header.
        // Elf64_Shdr *sh is a pointer to the current section header
        Elf64_Shdr *sh = &section_headers[i];
        const char *name = shstrtab ? (shstrtab + sh->sh_name) : "<no name>";

        printf("  [%2d] %-17s %-16s %016lx  %08lx\n", i, name,
               (sh->sh_type < SHT_NUM) ? elf_section_type_names[sh->sh_type] : "UNKNOWN",
               sh->sh_addr, sh->sh_offset);
        printf("       %016lx  %016lx  ", sh->sh_size, sh->sh_entsize);
        print_section_flags(sh->sh_flags);
        printf(" %5d %5d %5ld\n", sh->sh_link, sh->sh_info, sh->sh_addralign);
    }
	
    //This line to print the flag descriptions
    print_flag_descriptions();


    free(section_headers);
    free(shstrtab);
}








int main(int argc, char *argv[])
{	
	// Flags to indicate whether to print the ELF header (header_flag) or section headers (sections_flag) or both of them(when both falgs are TRUE). 
    int header_flag = FALSE, sections_flag = FALSE;

    // This variable will store the return value of getopt, which indicates the current option status being processed.	
	int opt;
	
	// optind is used by getopt to track the index of the next element of the argv array to be processed. It is automatically initialized to 1 and updated by getopt.
    extern int optind;

    // Parse command-line options
	// If an invalid option(That starts with '-') is encountered (not in optstring("-h -s")), getopt returns '?'.
	// If it No more options starting with - to process getopt returns -1, ending the loop.
    while ((opt = getopt(argc, argv, "hs")) != ALL_OPTIONS_ARE_PARSED)
	{
        switch (opt) 
		{
            case 'h':
                header_flag = TRUE;
                break;
            case 's':
                sections_flag = TRUE;
                break;
            default:
                fprintf(stderr, "Usage: %s [-h] [-s] <ELF file>\n", argv[0]); // If an unknown option is encountered, print To user how to print command and return 
                return EXIT_FAILURE;
        }
    }

    // Check for ELF file argument
	// If optind == argc it means user enter name of program only without elf-file
    if (optind >= argc) 
	{
        fprintf(stderr, "Expected ELF file name after options\n");
        return EXIT_FAILURE;
    }

    // After parsing options(looping on getopt() function), optind will point to the first non-option argument, which should be the filename in this case.
    const char *filename = argv[optind];
	
	// Open the ELF file in binary read mode
    FILE *file = fopen(filename, "rb"); //If the file cannot be opened, print an error message and return EXIT_FAILURE.
    if (!file) 
	{
		//If fopen fails, fprintf prints an error message to the standard error stream (stderr).
		//strerror(errno): Converts the error code (errno) to a human-readable string. 
		//errno is set by the system when a function like fopen fails, indicating the error that occurred.
        fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
        return EXIT_FAILURE;
    }

    // If fread does not read exactly one item of size sizeof(header) from the beginning of elf-file, print an error message, close the file, and return EXIT_FAILURE.
	// fread returns the number of elements successfully read.
	// If fread reads exactly one element of size sizeof(header), it returns 1. 
	// If it reads fewer than 1 element (indicating an error or end-of-file), it returns a value less than 1.
    Elf64_Ehdr header;
    if (fread(&header, sizeof(header), 1, file) != SUCCESS_OPERATION) 
	{
        fprintf(stderr, "Error reading ELF header: %s\n", strerror(errno));
        fclose(file);
        return EXIT_FAILURE;
    }

    // Verify the ELF magic number
	// This condition compares the first SELFMAG (4) bytes of header.e_ident with ELFMAG. If they are equal, it means the file is an ELF file.
	// If the result of memcmp is not zero, it means the first 4 bytes of header.e_ident do not match ELFMAG, So print an error message and return EXIT_FAILURE.
    if (memcmp(header.e_ident, ELFMAG, SELFMAG) != ARRAYS_ARE_EQUAL) 
	{
        fprintf(stderr, "Not an ELF file\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Print ELF header based on flag
	//Print header even if I don't enter "-h" argument and leave the option argument empty to the command
    if (header_flag || (!header_flag && !sections_flag)) 
	{
        print_elf_header(&header);
    }

    // Print Section headers based on flag
    if (sections_flag) 
	{
        print_section_headers(file, &header);
    }

    fclose(file);
    return EXIT_SUCCESS;
}

