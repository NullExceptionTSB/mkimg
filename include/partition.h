#pragma once
#include <stdlib.h>
#include <chs.h>
#include <filesystem.h>

typedef enum _MKIMG_DESIRED_PARTITION_TYPE {
    PARTTYPE_UNDECIDED, PARTTYPE_NONE, PARTTYPE_MBR, PARTTYPE_GPT
}mkimg_parttype;

typedef struct _MKIMG_PARTITION {
    size_t partition_size;
    chs partition_size_chs;
    char* partition_buffer;
    mkimg_filesystem partition_file_system;
}partition;