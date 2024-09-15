# Myls Implementation

This repository presents a custom implementation of the Unix `ls` command, which lists directory contents. The custom version supports a variety of options, similar to the original `ls` command, with additional features like colorful text output for better readability.

## Supported Options

The custom implementation of `ls` supports the following options:

- **`-l`**: Display detailed information about each file.
- **`-a`**: Include hidden files in the listing.
- **`-t`**: Sort files by modification time, newest first.
- **`-tu`**: Display access time, sorted by access time.
- **`-tc`**: Display change time, sorted by change time.
- **`-i`**: Show inode number at the beginning of each file entry.
- **`-f`**: Disable sorting and long format; include hidden files.
- **`-d`**: List directories themselves, rather than their contents.
- **`-1`**: List one file per line.



## Compilation and Execution

To compile the program, use the provided command:

```bash
gcc -g Myls.c Helper.c Option_Handler.c -o myls
```

To run the custom `ls` command, use the following syntax:

```bash
./myls [options] [file...]
```

Replace `[options]` with the desired flags (e.g., `-l`, `-a`), and `[file...]` with the directory or file paths you wish to list. If no files are provided, the current directory will be listed by default.



## Output Samples

### ls to show diffrent directories contents
![image_2024-09-15_03-34-11](https://github.com/user-attachments/assets/b3dd6fc5-5922-47cd-a23b-59f882337d59)

### Long Format(myls -l)
![image_2024-09-14_22-42-58](https://github.com/user-attachments/assets/8b954de7-b538-4b22-88fd-78e153d57db9)

### Show hidden files(myls -a)
![image_2024-09-14_20-30-21](https://github.com/user-attachments/assets/69e297fc-1973-4548-978d-5fb7d5c56af4)

### sort by modification time(myls -t)
![image_2024-09-14_20-23-50](https://github.com/user-attachments/assets/7cc864b5-4374-46f8-8bc2-0d46a4f9b0d1)

### Sort by access time(myls -tu)
![image_2024-09-14_22-28-06](https://github.com/user-attachments/assets/c8a27c23-a8fd-456c-be17-05313ebaece3)

### Sort by access time(myls -tc)
![image_2024-09-14_22-38-39](https://github.com/user-attachments/assets/9f17fcd9-6287-4dbb-94dc-ac1591d177c9)

### Show Inode number(myls -i)
![image_2024-09-14_22-53-56](https://github.com/user-attachments/assets/a2709346-75bd-4977-90b6-1f34d13a0e96)

### Disable options(myls -f[any other option])
![image_2024-09-14_23-42-00](https://github.com/user-attachments/assets/4ea03f6e-cc51-4071-89d0-abf75ac075a1)

### Show passed directories only(myls -d)
![image_2024-09-14_23-30-33](https://github.com/user-attachments/assets/99b5b742-70e0-432e-bb25-6265264d0c0d

### View each file in a row(myls -1)
![image_2024-09-14_23-33-19](https://github.com/user-attachments/assets/96def46a-d9d7-49f1-8606-21c69572c12c)


## Special Permissions

1. **SetUID (Set User ID on Execution):**
   This permission allows a file to be executed with the privileges of the file owner, rather than the user who is running the file. It is commonly used for executables that require elevated privileges for certain operations.

2. **SetGID (Set Group ID on Execution):**
   This permission enables a file to be executed with the group privileges of the file, instead of the group of the user running it. For directories, this permission ensures that new files created within inherit the directory’s group ownership.

3. **Sticky Bit:**
   The sticky bit is typically used on directories. It ensures that only the file owner, the directory owner, or the root user can delete or rename files within that directory. It does not affect file execution but is a security mechanism to control file manipulation in shared directories.


### SetUID with Execution Permission

When the `setuid` permission is applied to a file that has execution permissions, the letter `s` appears in lowercase in the owner's execution bit of the file's permissions. This indicates that the file will be executed with the owner's privileges. 

For example, in the file `set_uid_file`, you will see the `s` in place of the usual execution (`x`) bit, showing that the `setuid` is enabled.
![image_2024-09-15_00-35-19](https://github.com/user-attachments/assets/fa94d6e0-8e3d-4b58-8a72-9957d5eaa3e4)



### SetUID Without Execution Permission

When the `setuid` permission is applied to a file that does **not** have execution permission, an uppercase `S` appears in place of the owner's execution bit in the file’s permission settings. This indicates that while the `setuid` bit is set, the file itself does not have execution permission.

For example, in the file `set_uid_file`, the uppercase `S` will appear in the third bit of the owner's permissions, signifying that the `setuid` is enabled, but the file cannot be executed.
![image_2024-09-15_00-42-18](https://github.com/user-attachments/assets/6938f41a-180e-465c-9f35-16578ec88569)



### SetGID With Execution Permission

When the `setgid` permission is applied to a file **with** execution permission, a lowercase `s` appears in place of the group's execution bit in the file’s permission settings. This indicates that the file will be executed with the group permissions of the file, not the group of the user running it.

For example, in the file `set_uid_file`, the lowercase `s` will appear in the third bit of the group’s permissions, indicating that the `setgid` bit is set, and the file has execution permission.
![image_2024-09-15_00-44-50](https://github.com/user-attachments/assets/7e0a1ce7-608d-458a-8dd1-b91b43a32da6)


### SetGID Without Execution Permission

When the `setgid` permission is applied to a file **without** execution permission, an uppercase `S` appears in place of the group's execution bit in the file’s permission settings. This indicates that the `setgid` bit is set, but the file does not have execution permission.

For example, in the file `set_uid_file`, the uppercase `S` will appear in the third bit of the group’s permissions, signifying that the `setgid` is enabled, but the file cannot be executed.
![image_2024-09-15_00-48-16](https://github.com/user-attachments/assets/ed6208a6-bc04-470c-8a1c-14fa1f8a4827)


### Sticky Bit With Execution Permission

When the sticky bit is applied to a file **with** execution permission, a lowercase `t` appears in place of the other's execution bit in the file’s permission settings. This indicates that the sticky bit is set and the file has execution permission.

For example, in the file `set_uid_file`, the lowercase `t` will appear in the third bit of the other's permissions, indicating that the sticky bit is enabled and the file can be executed.
![image_2024-09-15_02-48-56](https://github.com/user-attachments/assets/9ddfdc7d-65e8-4f5f-ae34-3bf6847501d8)


### Sticky Bit Without Execution Permission

When the sticky bit is set **without** execution permission, a uppercase `T` appears in the third bit of the other's permissions in the file’s permission settings. This signifies that the sticky bit is active and the file has execution permission.

For instance, in the file `set_uid_file`, the upper `T` will appear in the third position of the other’s permissions, indicating that both the sticky bit is set, and the file can be executed.
![image_2024-09-15_02-52-06](https://github.com/user-attachments/assets/32948287-7c5a-4b05-91d6-ecac0fe320e4)



