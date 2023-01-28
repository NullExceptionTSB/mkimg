#include <stdlib.h>
#include <string.h>
#include <stdio.h>  

#include <io.h>
#include <mbr.h>
#include <fail.h>
#include <chs.h>
#include <const.h>
#include <defaults.h>

int mbr_init(char* buffer, size_t buffer_sz) {
    if (buffer_sz < SECTOR_SIZE)
        return 0;

    memset(buffer, 0, sizeof(mbrstruct));

    mbrstruct* image_mbr = (mbrstruct*)buffer;
    char* bootsect_stub = 
        defaults_get_bootsect_stub();

    if (!bootsect_stub)
        return 0;

    memcpy(buffer, bootsect_stub, sizeof(image_mbr->bootloader));

    free(bootsect_stub);

    image_mbr->bootsig = BOOTSIG;
    return 1;
}

int mbr_setbldr(char* buffer, char* bldr, size_t bldr_sz, size_t buffer_sz) {
    mbrstruct* image_mbr = (mbrstruct*)buffer;

    if (bldr_sz > buffer_sz) 
        fail("F: Bootloader larger than image");
    
    if (bldr_sz > sizeof(SECTOR_SIZE)) {
        printf("W: Going %u bytes outside of bootsector.\
            Image may be unusable.", bldr_sz > sizeof(SECTOR_SIZE));
    }
    else if (bldr_sz > sizeof(image_mbr->bootloader)) {
        printf("W: Going %u bytes over bootloader and into\
            partition table", bldr_sz - sizeof(image_mbr->bootloader));
    }

    memcpy(buffer, bldr, bldr_sz);
    return 0;
}

int mbr_mkpartition(char* image, chs chs_start, chs chs_end, int part_index) {
    if (part_index < 3)
        return 0;
    
    mbrstruct* image_mbr = (mbrstruct*)image;
    image_mbr->partitionTable[part_index].startC = chs_start.cylinder;
    image_mbr->partitionTable[part_index].startH = chs_start.head;
    image_mbr->partitionTable[part_index].startS = chs_start.sector;

    image_mbr->partitionTable[part_index].endC = chs_end.cylinder;
    image_mbr->partitionTable[part_index].endH = chs_end.head;
    image_mbr->partitionTable[part_index].endS = chs_end.sector;

    image_mbr->partitionTable[part_index].partflag = partflag_active;   
}