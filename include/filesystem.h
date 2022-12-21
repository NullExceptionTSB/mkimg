#pragma once
#include <stdint.h>

typedef enum _MKIMG_FILESYSTEM_STRUCT {
    FSNone,
    FSAuto, 
    FSFAT12, 
    FSFAT16, 
    FSFAT32
} mkimg_filesystem;