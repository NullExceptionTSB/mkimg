#include <string.h>
#include <stdlib.h>

#include <arg.h>
#include <fat.h>
#include <mbr.h>
#include <image.h>

int fat12_stub_value() {
    return 0x9;
}

int fat12_calc_sectors_per_fat(bpb16* bpb) {
    //generic values
    switch (bpb->totalSectors) {
        case 2880: return 9;
    }

    //calculate if undefined
    return -1; //stub

}

void fat12_relabel(bpb16* bpb, char* newlabel) {
    memcpy(bpb->label, newlabel, strnlen(newlabel, 11));
}

bpb16* fat12_write_bpb_direct(image* img) {
    bpb16* bpb = calloc(1, sizeof(bpb16));
    bpb->bytesPerSector = 512;
    memset(bpb->oem, ' ', 8);
    bpb->sectorsPerCluster = 1; //forced for fat12
    bpb->reservedSectors = 1;
    bpb->numFats = 2;
    bpb->rootDirEntries = 224; //FLOPPY ONLY! HARD DRIVES USE 512
    bpb->totalSectors = img->image_size/bpb->bytesPerSector;
    bpb->mediaDescriptor = 0xF0; //FLOPPY ONLY!
    bpb->sectorsPerFat = fat12_stub_value();

    bpb->sectorsPerTrack = img->image_size_chs.spt;
    bpb->heads = img->image_size_chs.head;
    bpb->hiddenSectors = 0;
    bpb->largeSectors = 0;

    bpb->driveNum = 0;
    bpb->flags = 0;
    bpb->bootsigex = FAT_BOOT_SIG_EX;
    bpb->serial = 0x0BADBEEF;
    fat12_relabel(bpb, "NOLABEL");
    memcpy(bpb->filesystem, "FAT12   ", 8);

    memcpy(img->image_buffer, bpb, sizeof(bpb16));
    return bpb;
}

void fat12_puttables(char* ptr, int table_size_sectors, int num_tables) {
    memset(ptr, 0, table_size_sectors*512*num_tables);

    for (int i = 0; i < num_tables; i++) {
        (ptr+(table_size_sectors*i))[0] = 0xF0;
        (ptr+(table_size_sectors*i))[1] = 0xFF;
        (ptr+(table_size_sectors*i))[2] = 0xFF;
    }
}

void fat12_putroot(char* ptr, int entries) {
    memset(ptr, 0, entries*32);
}

void fat12_format(image* img) {
    if (img->image_size < 512) {
        puts("F: Image too small");
        return;
    }

    switch (img->image_partition_table) {
        case PARTTYPE_NONE:
            bpb16* bpb = fat12_write_bpb_direct(img->image_buffer);
            fat12_puttables(img->image_buffer + 
                bpb->reservedSectors*bpb->bytesPerSector,
                bpb->sectorsPerFat, bpb->numFats);

            break;
        default:
            puts("F: Unsupported partition type, cannot format");
            return;
    }
}