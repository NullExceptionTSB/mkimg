#pragma once
#include <inttypes.h>
#define FAT_BOOT_SIG_EX 0x29
#pragma pack(push, 1)
typedef struct _BIOS_PARAMETER_BLOCK {
    uint8_t  jumpcode[3];
    char     oem[8];
    uint16_t bytesPerSector;
    uint8_t  sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t  numFats;
    uint16_t rootDirEntries;
    uint16_t totalSectors;
    uint8_t  mediaDescriptor;
    uint16_t sectorsPerFat;

    uint16_t sectorsPerTrack;
    uint16_t heads;
    uint32_t hiddenSectors;
    uint32_t largeSectors;

    uint8_t  driveNum;
    uint8_t  flags;
    uint8_t  bootsigex;
    uint32_t serial;
    char     label[11];
    char     filesystem[8];
}bpb16;
#pragma pack(pop)