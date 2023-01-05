# mkimg
Hard drive and floppy disk image builder tool, intended primarily for use in operating system development
and to replace platform-specific and root dependent tools (mount) with a single program\
![GNU GPLv3 Logo with "Free as in Freedom"](https://www.gnu.org/graphics/gplv3-with-text-136x68.png)
<br>
- Written in C99
- Image creation wtih both LBA and CHS parameters
- Free software (Licensed under GPLv3-or-later)
- Platform independent (Windows via MSYS2)
- Root access and mounting not required

 
## Usage
For basic usage instructions, run `mkimg --help`<br>
### Templates
This is a list for current templates supported by the `-t/--template` option:
- 1 = 1.44MiB 3.5' Floppy Disk
    - 2 heads per cylinder
    - 18 sectors per track
    - FAT12, 9 sectors per FAT, 2 FATs
    - no partition table (implicit -u)
### Usage example
To create a 1.44MiB 3.5' Floppy Disk image named `FloppyImageFile.img`:\
`mkimg -c -t 1 -o FloppyImageFile.img`\
**NOTE: THIS COMMAND WILL NOT CURRENTLY WORK AS FS DETECTION IS NOT CURRENTLY IMPLEMENTED**
To add file `main.c` to image `FloppyImageFile.img` which is unpartitioned:\
`mkimg -a -i main.c -o FloppyImageFile.img -u`\
To add file `main.c` to image `FloppyImageFile.img` which is a 1.44MiB 3.5' Floppy Disk:\
`mkimg -a -t 1 -i main.c -o FloppyImageFile.img`

## COMPILATION INSTRUCTIONS:
TO COMPILE: `make`, executable will be outputted in project root directory\
TO INSTALL: `sudo make install`, must be compiled **before** being installed!!! 
