#include <stdlib.h>
#include <string.h>

#include <image.h>
#include <io.h>

image* image_new() {
    image* img = malloc(sizeof(image));
    if (!img) return NULL;

    memset(img, 0, sizeof(image));
    return img;
}

void image_new_buffer(image* img, size_t buffersz) {
    img->image_size = buffersz;
    img->image_size_chs = chs_lba_to_chs_size(buffersz);
    //if (img->image_buffer) free(img->image_buffer);

    img->image_buffer = malloc(img->image_size);
}

void image_new_buffer_chs(image* img, chs chs_sz) {
    memcpy(&(img->image_size_chs), &chs_sz, sizeof(chs));
    img->image_size = chs_sz.cylinder*chs_sz.head*chs_sz.spt*512;
    if (img->image_buffer) free(img->image_buffer);

    void* buffer = malloc(img->image_size);
    img->image_buffer = buffer;
}

void image_detect(image* img) {
    img->image_file_system = FSNone;
    img->image_partition_table = PARTTYPE_NONE;
}

void image_free(image* img) {
    if (img->image_buffer) 
        free(img->image_buffer);

    free(img);
}