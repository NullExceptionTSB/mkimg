#pragma once
#include <image.h>
#include <filesystem.h>
void fat12_format(image* img);
void fat12_set_bootsect(char* bootsector, size_t bs_size, image* img);
void fat12_addfile(char* filename, char* data, size_t data_size, image* img);

extern const mkimg_fsdriver fsdriver_fat12;