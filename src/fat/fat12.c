#include <string.h>

#include <fail.h>
#include <partition.h>

#include <fs/fat.h>
#include <fs/fat12.h>
#include <driver/fsdriver.h>


int fat12_calc_sectors_per_fat(bpb16* bpb) {
    //generic values
    switch (bpb->totalSectors) {
        case 2880: return 9;
        //todo: expand
    }

    //calculate if undefined
    return -1; //stub
}

bpb16* fat12_write_bpb(partition* part, int small_root) {
    bpb16* bpb = calloc(1, sizeof(bpb16));
    bpb->jumpcode[0] = 0xEB;
    bpb->jumpcode[1] = sizeof(bpb16)-2;
    bpb->jumpcode[2] = 0x90;

    bpb->bytesPerSector = 512;
    memset(bpb->oem, ' ', 8);
    memcpy(bpb->oem, "mkimg", 5);
    bpb->sectorsPerCluster = 1; //forced for fat12
    bpb->reservedSectors = 1;
    bpb->numFats = 2;
    //FLOPPY ONLY! HARD DRIVES USE 512
    bpb->rootDirEntries = small_root ? 224 : 512; 
    bpb->totalSectors = part->partition_size/bpb->bytesPerSector;
    bpb->mediaDescriptor = 0xF0; //FLOPPY ONLY!
    
    bpb->sectorsPerTrack = part->partition_size_chs.spt;
    bpb->heads = part->partition_size_chs.head;
    bpb->hiddenSectors = 0;
    bpb->largeSectors = 0;

    bpb->driveNum = 0;
    bpb->flags = 0;
    bpb->bootsigex = FAT_BOOT_SIG_EX;
    bpb->serial = 0x0BADBEEF;
    fat_relabel(bpb, "NOLABEL");
    memcpy(bpb->filesystem, "FAT12   ", 8);

    bpb->sectorsPerFat = fat12_calc_sectors_per_fat(bpb);

    if (bpb->sectorsPerFat == -1) 
        fail("F: Stub reached");
        
    memcpy(part->partition_buffer, bpb, sizeof(bpb16));
    return bpb;
}
void fat12_puttables(char* ptr, int table_size_sectors, int num_tables) {
    memset(ptr, 0, table_size_sectors*512*num_tables);
    for (int i = 0; i < num_tables; i++) {
        (ptr+(table_size_sectors*512*i))[0] = 0xF0;
        (ptr+(table_size_sectors*512*i))[1] = 0xFF;
        (ptr+(table_size_sectors*512*i))[2] = 0xFF;   
    }
}
void fat12_putdir(char* ptr, int entries) {
    memset(ptr, 0, entries*32);
}
//external functions

void fat12_set_bootsect(char* bootsector_data, size_t bssize,
    partition* part, int seek_sector) {
    size_t seekc = sizeof(bpb16);
    if (seek_sector) 
        memcpy(part->partition_buffer + seekc, bootsector_data + seekc, 
            bssize-seekc);
    else 
        memcpy(part->partition_buffer + seekc, bootsector_data, bssize-seekc);
}
/*
void fat12_set_bootesect(char* bootsector_data, size_t bssize,
    image* img, int seek_sector) {
    size_t seekc = sizeof(bpb16);
    if (seek_sector) 
        memcpy(img->image_buffer + seekc, bootsector_data + seekc, 
            bssize-seekc);
    else 
        memcpy(img->image_buffer + seekc, bootsector_data, bssize-seekc);
}
*/

void fat12_format(partition* part, mkimg_args* args) {
    bpb16* bpb = fat12_write_bpb(part, args->create_fat_small_root);
    fat12_puttables(part->partition_buffer + 
        bpb->reservedSectors*bpb->bytesPerSector,
        bpb->sectorsPerFat, bpb->numFats);
    fat12_putdir(part->partition_buffer + 
        bpb->reservedSectors*bpb->bytesPerSector+
        bpb->sectorsPerFat*bpb->numFats*bpb->bytesPerSector,
        bpb->rootDirEntries);
}

const mkimg_fsdriver fsdriver_fat12 = 
    {"FAT12", FSFAT12, 
        NULL, 
        fat12_format, 
        fat12_set_bootsect };