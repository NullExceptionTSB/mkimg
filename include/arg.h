#pragma once
#include <filesystem.h>
#include <partition.h>

typedef enum _MKIMG_ARGUMENT_MODE {
    MODE_UNDECIDED, create, cpfile, rmfile
} mkimg_mode;

typedef enum _MKIMG_SIZE_MODE {
    SIZEMODE_UNDECIDED, CHS, LBA
} mkimg_sizemode;

typedef struct _MKIMG_ARGUMENT_STRUCT {
    mkimg_mode mode;

    unsigned char verbose : 1;
    unsigned char bsnoseek : 1;
    unsigned char reserved : 6;

    char* outfile;
    char* infile;

    int create_template;
    int create_fat_small_root;
    int create_sz_cylinders;
    int create_sz_heads;
    int create_sz_spt;
    int create_sz_lba;
    mkimg_sizemode create_sizemode;
    mkimg_filesystem create_desiredfs;
    mkimg_parttype partition_type;

} mkimg_args;

mkimg_args* arg_parse(int argc, char* argv[]);
void arg_fail(char* msg);