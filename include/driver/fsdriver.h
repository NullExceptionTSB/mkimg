#pragma once
#include <stdint.h>
#include <stddef.h>
#include <arg.h>
#include <image.h>
#include <partition.h>
#include <filesystem.h>

typedef void(*fs_add_file_proc)(char* filename, char* data,
    size_t data_size, image* img);
typedef void(*fs_format_proc)(partition* img, mkimg_args* args);
typedef void(*fs_set_boot_sector_proc)(char* bootsector_data, size_t bssize,
    partition* part, int seek_sector);

typedef struct _MKIMG_FS_DRIVER {
    char* fsname;
    mkimg_filesystem fsenum;
    fs_add_file_proc add_file;
    fs_format_proc format;
    fs_set_boot_sector_proc set_boot_sector;
} mkimg_fsdriver;

mkimg_fsdriver* filesystem_enum_to_driver(mkimg_filesystem fsenum);