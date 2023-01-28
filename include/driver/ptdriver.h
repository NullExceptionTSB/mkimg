#pragma once
#include <partition.h>
#include <image.h>

typedef void(*pt_parse)(image* img);
typedef void(*pt_set_mbr)(char* bootsector_data, size_t bssize,
    image* img, int seek_sector);
typedef void(*pt_add)(image* img, size_t part_start, size_t part_size);
typedef void(*pt_init)(image* img);

typedef struct _MKIMG_PT_DRIVER {
    char* ptname;
    mkimg_parttype ptenum;
    pt_parse parse;
    pt_set_mbr set_mbr;
    pt_add add;
    pt_init init;
}mkimg_ptdriver;

mkimg_ptdriver* partition_enum_to_driver(mkimg_parttype ptenum);