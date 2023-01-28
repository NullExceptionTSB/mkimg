#pragma once

#include <driver/ptdriver.h>
#include <pt/unpart.h>

const mkimg_ptdriver* known_ptdriver_table[] = {
    NULL, //PARTYPE_UNDECIDED
    &ptdriver_unpart, //PARTTYPE_UNPARTITIONED
    NULL, //PARTTYPE_MBR
    NULL, //PARTTYPE_GPT
};