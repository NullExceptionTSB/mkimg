#pragma once
#include <inttypes.h>

#pragma pack(push, 1)
#define BOOTSIG 0xAA55

typedef enum _PARTITION_FLAG {
    partflag_inactive = 0x00,
    partflag_active =   0x80
} partflag;

typedef struct _PARTITION_ENTRY {
    uint32_t partflag : 8;
    uint32_t startH : 8;
    uint32_t startS : 6;
    uint32_t startC : 10;

    uint32_t parttype : 8;
    uint32_t endH   : 8;
    uint32_t endS   : 6;
    uint32_t endC   : 10;
    uint32_t startLBA;
    uint32_t sectorLen;
}partentry;

typedef struct _MASTER_BOOT_RECORD {
    uint8_t bootloader[446];
    partentry partitionTable[4];
    uint16_t bootsig;
}mbrstruct;
#pragma pack(pop)