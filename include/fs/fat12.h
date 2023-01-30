#pragma once
#include <image.h>
#include <filesystem.h>
#include <driver/fsdriver.h>

#define FAT12_FORCE_SMALL_ROOT 1

typedef struct _FAT12_CLUSTER {
    uint8_t     even_clust1;
    uint8_t     even_clust2 : 4;
    uint8_t     odd_clust1  : 4;
    uint8_t     odd_clust2;
}fat12_cluster;

extern const mkimg_fsdriver fsdriver_fat12;