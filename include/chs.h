#pragma once
#include <inttypes.h>
#include <stdlib.h>

typedef struct _MKIMG_CHS {
    uint16_t cylinder;
    uint8_t head;
    union {
        uint8_t sector;
        uint8_t spt;
    };
}chs;

chs chs_lba_to_chs_size(size_t sz);
chs chs_lba_to_chs_addr(size_t lba, void* imge);
size_t chs_chs_to_lba_size(chs size);
size_t chs_chs_to_lba_addr(chs addr, void* imge);