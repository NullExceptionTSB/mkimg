#pragma once
#include <inttypes.h>
#define FAT_BOOT_SIG_EX 0x29
#pragma pack(push, 1)
typedef struct _BIOS_PARAMETER_BLOCK {
    uint8_t     jumpcode[3];
    char        oem[8];
    uint16_t    bytesPerSector;
    uint8_t     sectorsPerCluster;
    uint16_t    reservedSectors;
    uint8_t     numFats;
    uint16_t    rootDirEntries;
    uint16_t    totalSectors;
    uint8_t     mediaDescriptor;
    uint16_t    sectorsPerFat;

    uint16_t    sectorsPerTrack;
    uint16_t    heads;
    uint32_t    hiddenSectors;
    uint32_t    largeSectors;

    uint8_t     driveNum;
    uint8_t     flags;
    uint8_t     bootsigex;
    uint32_t    serial;
    char        label[11];
    char        filesystem[8];
}bpb16;

typedef struct _FAT12_CLUSTER {
    uint8_t     even_clust1;
    uint8_t     even_clust2 : 4;
    uint8_t     odd_clust1  : 4;
    uint8_t     odd_clust2;
}fat12_cluster;

typedef struct _FAT_ROOT_DIR_ENTRY {
    char        filename[8];
    char        extension[3];
    uint8_t     attribute; 
    uint8_t     attrib_ex;   
    uint8_t     creation_time_ms;
    uint16_t    creation_time;
    uint16_t    creation_date;
    uint16_t    last_access_date;
    uint16_t    reserved;
    uint16_t    last_modification_time;
    uint16_t    last_modification_date;
    uint16_t    start_cluster;
    uint32_t    file_size;
}rootdir_entry;
#pragma pack(pop)

void fat_relabel(bpb16* bpb, char* newlabel);
char* fat_new_short_filename(char* long_filename);
void fat_putdir(char* ptr, int entries);
int fat_calc_spf(int clus_size_bits, int clus_size_sect, 
    int res_sect, int rootdir_entries, int nfats, int total_sect);