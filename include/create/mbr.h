#pragma once
#include <stdlib.h>

#include <chs.h>
#include <mbr.h>
int mbr_init(char* buffer, size_t buffer_sz);
int mbr_setbldr(char* buffer, char* bldr, size_t bldr_sz, size_t buffer_sz);
int mbr_mkpartition(char* image, chs chs_start, chs chs_end, int part_index);