#pragma once
#include <stdlib.h>
#include <chs.h>
#include <arg.h>
#include <partition.h>

/*
typedef struct _MKIMG_DISK {
    chs image_size_chs;
    size_t image_size;
    char* image_buffer;
    mkimg_parttype image_partition_table;
    mkimg_filesystem image_file_system;
    int image_template;
}image;
*/

typedef struct _MKIMG_IMAGE {
    chs image_size_chs;
    size_t image_size;
    char* image_buffer;
    size_t partition_count;
    partition* partitions;
    mkimg_parttype image_partition_table;
    int image_template;
}image;

image* image_new();
void image_new_buffer(image* img, size_t buffersz);
void image_new_buffer_chs(image* img, chs chs_sz);
void image_detect(image* img);
void image_detect_ex(image* img, int force_unpart);
void image_load(image* img, char* path);
void image_free(image* img);