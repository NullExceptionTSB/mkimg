#include <stdio.h>
#include <stdlib.h>

#include <filesystem.h>
#include <fsdriver.h>
#include <io.h>
#include <arg.h>
#include <image.h>

#include <create/mbr.h>

void fail(char* msg) {
    puts(msg);
    exit(-1);
}

void mode_create(mkimg_args* args) {
    image* img = image_new();
    image_detect(img);
    img->image_template = args->create_template;
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

    mkimg_fsdriver* fsdriver = 
        filesystem_enum_to_driver(args->create_desiredfs);
    if (!fsdriver)
        fail("F: Filesystem unknown or unsupported");
    if (!fsdriver->format)
        fail("F: Filesystem driver does not support formatting");
    if (!fsdriver->set_boot_sector)
        puts("W: Filesystem driver does not support bootloader writing");

    fsdriver->format(img);
    if (args->infile && fsdriver->set_boot_sector) {
        size_t sz = io_get_file_size(args->infile);
        char* bs = io_read_file(args->infile, sz, &sz);
        fsdriver->set_boot_sector(bs,sz,img);
        free(bs);
    }

    io_write_file(args->outfile, img->image_buffer, img->image_size);

    image_free(img);
}

void mode_add(mkimg_args* args) {
    image* img = image_new();
    image_load(img, args->outfile);
    
    if (!img->image_buffer) 
        fail("F: Failed to open image file");

    image_detect_ex(img, args->force_unpartitioned>0, args->force_nofs>0);
    if (args->create_desiredfs)
        img->image_file_system = args->create_desiredfs;

    mkimg_fsdriver* fsdriver = 
        filesystem_enum_to_driver(img->image_file_system);
    if (!fsdriver)
        fail("F: Filesystem unknown or unsupported");
    
    if (!fsdriver->add_file)
        fail("F: Filesystem driver does not support writing");

    size_t file_sz = io_get_file_size(args->infile);
    char* file_data = io_read_file(args->infile, file_sz, &file_sz);
    if (!file_data)
        fail("F: Failed to open input file");
        
    fsdriver->add_file(args->infile, file_data, file_sz, img);
    
    io_write_file(args->outfile, img->image_buffer, img->image_size);

    image_free(img);
    free(file_data);
}

int main(int argc, char* argv[]) {
    mkimg_args* args = arg_parse(argc, argv);
    switch (args->mode) {
        case create:
            mode_create(args);
            break;
        case cpfile:
            mode_add(args);
            break;
        default: fail("F: Mode not supported");
    }
    return 0;
}