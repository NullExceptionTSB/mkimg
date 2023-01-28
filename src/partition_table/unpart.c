#include <string.h>
#include <stdlib.h>
#include <driver/ptdriver.h>

void pt_unpart_parse(image* img) {
    img->partition_count = 1;
    img->partitions = malloc(sizeof(partition));
    memset(img->partitions, 0, sizeof(partition));
    img->partitions->partition_buffer = img->image_buffer;
    img->partitions->partition_size = img->image_size;
    img->partitions->partition_size_chs = img->image_size_chs;
}

void pt_unpart_setmbr(char* bootsector_data, size_t bssize,
    image* img, int seek_sector) {return;}

void pt_unpart_add(image* img, size_t part_start, size_t part_size) {
    pt_unpart_parse(img);
}

void pt_unpart_init(image* img) {
    img->image_buffer[510] = 0x55;
    img->image_buffer[511] = 0xAA;
}

const mkimg_ptdriver ptdriver_unpart = {
    "Unpartitioned",
    PARTTYPE_NONE,
    pt_unpart_parse,
    pt_unpart_setmbr,
    pt_unpart_add,
    pt_unpart_init
};