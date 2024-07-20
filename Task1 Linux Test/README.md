# ELF Parser

ELF Parser is a command-line tool designed to analyze and display information about ELF (Executable and Linkable Format) files. This tool provides detailed insights into the structure of ELF files, including their headers and section information.

## Features

- Display ELF header information
- Show section headers and their details
- Support for both 32-bit and 64-bit ELF files
- Command-line options for customized output
- Can deal with most possible bugs

## Usage

```
./elf_parser [-h] [-s] <ELF file>
```

Options:
- `-h`: Display ELF header information
- `-s`: Display section headers
- `-s` `-h`: Display section headers and ELF header information

If no options are provided, the program defaults to displaying the ELF header.

## Example Outputs

### ELF Header and Section Headers

![Section_elf_header](https://github.com/user-attachments/assets/bba26cc2-2300-4123-835e-50d3b8525a07)

This image shows the output of running `./elf_parser -s -h file2.o`, which displays both the ELF header and section headers.

### Section Headers Only

![Section_Header](https://github.com/user-attachments/assets/4a06ae5e-45dc-49f3-a748-a4c04724f19d)

This image demonstrates the output when running `./elf_parser -s file2.o`, focusing solely on the section headers.

### Compilation and Various Usage Examples

![ELF_Header_Bugs_Managment](https://github.com/user-attachments/assets/7ab5e627-db2b-472e-a56d-b6c62cd162fb)

This image showcases the compilation process and various usage examples of the ELF Parser tool.

## Implementation Details

The ELF Parser is implemented in C and utilizes standard system libraries to read and interpret ELF file structures. Key aspects of the implementation include:

- Dynamic memory allocation for handling variable-sized data
- Robust error checking and reporting
- Support for different ELF file types (relocatable, executable, shared object)
- Detailed parsing of ELF headers and section information

## Building the Project

To compile the ELF Parser, use the following command:

```
gcc -o elf_parser elf_parser.c
```
