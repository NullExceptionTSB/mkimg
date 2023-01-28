#include <filesystem.h>
#include <driver/fsdriver.h>
#include <fsdtable.h>

//pointers returned by this function always point to read-only structures!
mkimg_fsdriver* filesystem_enum_to_driver(mkimg_filesystem fsenum) {
    if (fsenum == FSAuto || fsenum >= FSLast) 
        return NULL;
    return (mkimg_fsdriver*)known_fsdriver_table[fsenum];
}