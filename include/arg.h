#pragma once

typedef enum _MKIMG_ARGUMENT_MODE {
    MODE_UNDECIDED, create, cpfile, rmfile
} mkimg_mode;

typedef enum _MKIMG_SIZE_MODE {
    SIZEMODE_UNDECIDED, CHS, LBA
} mkimg_sizemode;

typedef enum _MKIMG_ARGUMENT_FILESYSTEM {
    NONE, FAT12, FAT16, FAT32, NTFS
} mkimg_filesystem;

typedef enum _MKIMG_DESIRED_PARTITION_TYPE {
    PARTTYPE_UNDECIDED, PARTTYPE_NONE, PARTTYPE_MBR, PARTTYPE_GPT
} mkimg_parttype;

typedef struct _MKIMG_ARGUMENT_STRUCT {
    mkimg_mode mode;
    char verbose : 1;
    char reserved : 7;
    char* outfile;
    char* infile;

    int create_sz_cylinders;
    int create_sz_heads;
    int create_sz_sectors;
    int create_sz_lba;
    mkimg_sizemode create_sizemode;
    mkimg_filesystem create_desiredfs;
    mkimg_parttype create_desiredparttype;

} mkimg_args;

mkimg_args* arg_parse(int argc, char* argv[]);