#include <stdlib.h>

#include <ptdtable.h>
#include <partition.h>

partition* partition_new() {
    partition* p = malloc(sizeof(partition));
}

mkimg_ptdriver* partition_enum_to_driver(mkimg_parttype ptenum) {
    return (mkimg_ptdriver*)known_ptdriver_table[ptenum];
}