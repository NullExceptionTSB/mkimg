#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <image.h>
#include <io.h>
#include <fail.h>

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

void image_detect_partition_table(image* img) {
    //detect GPT via magic number
    char gpt_mag_buff[8];
    memcpy(gpt_mag_buff, img->image_buffer, 8);
    if (!strncmp(gpt_mag_buff, "EFI PART", 8)) {
        img->image_partition_table = PARTTYPE_GPT;
        return;
    }
    //since it is nearly impossible to differentiate an unpartitioned medium
    //from an MBR partitioned medium, MBR is assumed unless the -u flag is
    //specified (including implicit -u via templates)
    img->image_partition_table = PARTTYPE_MBR;
}

void image_detect_ex(image* img, int force_unpart) {
    if (!img->image_buffer) {
        img->image_partition_table = PARTTYPE_NONE;
        return;
    }

    if (img->image_size < 512) 
        return;
    
    if (force_unpart) 
        img->image_partition_table = PARTTYPE_NONE;
    else 
        image_detect_partition_table(img);
}

void image_detect(image* img) { image_detect_ex(img, 0); }

void image_load(image* img, char* path) {
    size_t filesize = io_get_file_size(path);
    if (!filesize) return;

    char* imgbuffer = io_read_file(path, filesize, &filesize);
    if (!imgbuffer) return;

    img->image_buffer = imgbuffer;
    img->image_size = filesize;
}

void image_free(image* img) {
    if (img->image_buffer) 
        free(img->image_buffer);

    if (img->partitions) {
        for (int i = 0; i < img->partition_count; i++)
            free(&(img->partitions[i]));
        free(img->partitions);
    }

    free(img);
}