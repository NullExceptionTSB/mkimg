# mkimg
OSDev Image Builder Tool\
![GNU GPLv3 Logo with "Free as in Freedom"](https://www.gnu.org/graphics/gplv3-with-text-136x68.png)
<br>
- Written in C99
- Image creation wtih both LBA and CHS parameters
- Open-source
- Platform independent (Windows via MSYS2)
- Root access and mounting not required

 
## Usage
For basic usage instructions, run `mkimg --help`<br>
### Templates
This is a list for current templates supported by the `-t/--template` option:\
- 1 = 1.44MiB 3.5' Floppy Drive
- - 2 heads per cylinder
- - 18 sectors per track
- - FAT12, 9 sectors per FAT, 2 FATs


## COMPILATION INSTRUCTIONS:
TO COMPILE: `make`, executable will be outputted in project root directory\
TO INSTALL: `make install`, CURRENTLY DOES NOT WORK!!!
