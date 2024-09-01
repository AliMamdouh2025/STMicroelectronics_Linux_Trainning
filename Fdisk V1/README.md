
# Disk Partition Viewer

## Description

This project implements a simplified version of the `fdisk -l` command, designed to display disk partition information on Unix-like systems. It supports both MBR (Master Boot Record) and GPT (GUID Partition Table) partitioning schemes, providing detailed information about partitions on a given device.

## Features

- Supports both MBR and GPT partition tables
- Displays basic disk information (size, model, sector size)
- Shows detailed partition information including:
  - Device name
  - Partition index
  - Start sector
  - End sector
  - Total sectors
  - Size in MB
  - Partition type
- Handles logical partitions in MBR scheme
- Identifies common partition types for both MBR and GPT

## Requirements

- GCC compiler
- Linux-based operating system
- Root privileges (for accessing disk information)

## Compilation

To compile the program, navigate to the project directory and run:

```bash
gcc myfdisk.c GPT_Parsing.c MBR_Parsing.c -o myfdisk
```

## Usage

- Run the program with root privileges, specifying the device you want to analyze:

```bash
sudo ./myfdisk /dev/sdX
```

Replace `/dev/sdX` with the device you want to examine (e.g., `/dev/sda`, `/dev/nvme0n1`).

## Example Outputs

![image](https://github.com/user-attachments/assets/0adcf53a-88e7-4c7b-9572-f468191ca478)




## File Structure

- `main.c`: Main program logic
- `MBR.c` & `MBR.h`: MBR partition table parsing
- `GPT.c` & `GPT.h`: GPT partition table parsing




