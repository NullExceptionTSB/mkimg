#pragma once

typedef enum _MKIMG_FILESYSTEM_ENUM {
    FSNone = 0,
    FSAuto = -1, 
    FSFAT12 = 1, 
    FSFAT16, 
    FSFAT32,
    FSExFAT,
    FSNTFS,
    FSLast
} mkimg_filesystem;

