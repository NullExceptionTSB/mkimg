#include <stdio.h>
#include <stdlib.h>

#include <filesystem.h>
#include <io.h>
#include <arg.h>
#include <image.h>

#include <create/mbr.h>

#include <fs/fat12.h>

void fail(char* msg) {
    puts(msg);
    exit(-1);
}

void format(mkimg_args* args, image* img) {
    switch(args->create_desiredfs) {
        case FSAuto:
            puts("AutoFS not supported");
            break;
        case FSFAT12:
            fat12_format(img);
            break;
        default: puts("Filesystem not supported");
    }
}

void mode_create(mkimg_args* args) {
    image* img = image_new();
    image_detect(img);
    
    if (args->create_sizemode == LBA) 
        image_new_buffer(img, args->create_sz_lba);
    else {
        chs chssz;
        chssz.cylinder = args->create_sz_cylinders;
        chssz.head = args->create_sz_heads;
        chssz.spt = args->create_sz_spt;
        image_new_buffer_chs(img, chssz);
    }
    mbr_init(img->image_buffer, img->image_size);
    format(args, img);
    io_write_file(args->outfile, img->image_buffer, img->image_size);
}

int main(int argc, char* argv[]) {
    mkimg_args* args = arg_parse(argc, argv);

    if (args->mode != create)
        puts("F: Mode not supported");

    mode_create(args);
}