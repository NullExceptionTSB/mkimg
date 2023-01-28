#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <filesystem.h>
#include <driver/fsdriver.h>
#include <driver/ptdriver.h>

#include <io.h>
#include <arg.h>
#include <image.h>
#include <defaults.h>

void fail(char* msg) {
    puts(msg);
    exit(-1);
}

void mode_create(mkimg_args* args) {
    image* img = image_new();
    if (args->create_sizemode == LBA) 
        image_new_buffer(img, args->create_sz_lba);
    else {
        chs chssz;
        chssz.cylinder = args->create_sz_cylinders;
        chssz.head = args->create_sz_heads;
        chssz.spt = args->create_sz_spt;
        image_new_buffer_chs(img, chssz);
    }
    image_detect_ex(img, args->force_unpartitioned);
    
    img->image_partition_table = args->force_unpartitioned ?
        PARTTYPE_NONE :
        args->create_desiredparttype;
    img->image_template = args->create_template;

    mkimg_ptdriver* ptdriver = partition_enum_to_driver(img->image_partition_table);
    if (!ptdriver) 
        fail("F: unsupported partition table");
    ptdriver->init(img);
    if (ptdriver->add) ptdriver->add(img, 0, -1);

    mkimg_fsdriver* fsdriver = filesystem_enum_to_driver(args->create_desiredfs);
    if (!fsdriver)
        fail("F: unsupported file system");

    fsdriver->format(img->partitions, args);

    io_write_file(args->outfile, img->image_buffer, img->image_size);
}

int main(int argc, char* argv[]) {
    mkimg_args* args = arg_parse(argc, argv);

    switch (args->mode) {
        case create:
            mode_create(args);
            break;
        case cpfile:
            //mode_add(args);
            break;
        default: fail("F: Mode not supported");
    }
    return 0;
}