#pragma once
#include <image.h>
void fat12_format(image* img);
void fat12_set_bootsect(char* bootsector, size_t bs_size, image* img);
