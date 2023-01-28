#pragma once
#include <image.h>
#include <filesystem.h>
#include <driver/fsdriver.h>
#include <fs/fat12.h>

const mkimg_fsdriver* known_fsdriver_table[] = { 
    NULL, //FSNone
    &fsdriver_fat12, //FSFAT12
    NULL, //FSFAT16
    NULL, //FSFAT32
    NULL, //FSExFAT
    NULL, //FSNTFS 
};