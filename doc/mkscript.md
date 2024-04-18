# MKScript (for mkimg scripting mode)

## syntax
each command ends with a newline character (LF)

command an operands are separated with a space

commands are written in all caps

## command list

IMGFORMAT [raw]

sets the image format of the next CREATE command, currently does nothing
if this command is never specified, *raw* format is assumed


CREATE [template|size] [parameter]

creates an image


CREATE template [template-id]

creates an image by template


CREATE size [size]

creates an image with a specified size


MKLABEL [mbr|gpt] <label>

initializes an image's partition table


FORMAT [filesystem] <partition id>

formats the image with the specified file system

if partition id is specified, formats a partition instead



ADDFILE [path to file] <filename>

adds a file to the image's filesystem


RMFILE [filename]

deletes a file from the image's filesystem


BOOTSECTOR [path to bootsector] <extra parameters>

extra parameters:

SKIPBPB - don't overwrite the bios parameter block

SEEK [ammount] - seek over the first [ammount] bytes in the image

SET [ammount] - seek over the first [ammount] bytes in the bootsector

SKIP [ammount] - seek over the first [ammount] bytes in both the image nad bootsector

PARTITION [id] - sets a partition's bootsector instead

## additinal limitations

each line must be at most 1024 bytes long including the newline terminator

scripts must be encoded in ASCII