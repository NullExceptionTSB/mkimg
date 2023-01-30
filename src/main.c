#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <filesystem.h>
#include <driver/fsdriver.h>
#include <driver/ptdriver.h>

#include <arg.h>
#include <defaults.h>
#include <fail.h>
#include <io.h>
#include <image.h>

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
    image_detect_ex(img, args->partition_type == PARTTYPE_NONE);
    
    img->image_partition_table = args->partition_type;

    img->image_template = args->create_template;

    mkimg_ptdriver* ptdriver = partition_enum_to_driver(img->image_partition_table);
    fassert(ptdriver != 0,"F: unsupported partition table");

    ptdriver->init(img);
    if (ptdriver->add) ptdriver->add(img, 0, -1);

    mkimg_fsdriver* fsdriver = filesystem_enum_to_driver(args->create_desiredfs);
    fassert(fsdriver != 0, "F: unsupported file system");

    fsdriver->format(img->partitions, args);

    io_write_file(args->outfile, img->image_buffer, img->image_size);
}

void mode_add(mkimg_args* args) {
    image* img = image_new();
    image_load(img, args->outfile);

    if (args->partition_type == PARTTYPE_UNDECIDED)
        image_detect(img);
    else img->image_partition_table = args->partition_type;

    mkimg_ptdriver* ptdriver = partition_enum_to_driver(img->image_partition_table);
    fassert(ptdriver != 0,"F: unsupported partition table");
    ptdriver->parse(img);

    mkimg_fsdriver* fsdriver = filesystem_enum_to_driver(args->create_desiredfs);
    fassert(fsdriver != 0, "F: unsupported file system");

    size_t payload_size = 0;
    char* payload_data = io_read_file(args->infile, 
        io_get_file_size(args->infile), 
        &payload_size
    );
    
    fsdriver->add_file(args->infile, payload_data, payload_size, &(img->partitions[0]));

    io_write_file(args->outfile, img->image_buffer, img->image_size);
}

int mode_setbs(mkimg_args* args) {
    image* img = image_new();
    image_load(img, args->outfile);

    if (args->partition_type == PARTTYPE_UNDECIDED)
        image_detect(img);
    else img->image_partition_table = args->partition_type;

    mkimg_ptdriver* ptdriver = partition_enum_to_driver(img->image_partition_table);
    fassert(ptdriver != 0,"F: unsupported partition table");
    ptdriver->parse(img);

    mkimg_fsdriver* fsdriver = filesystem_enum_to_driver(args->create_desiredfs);
    fassert(fsdriver != 0, "F: unsupported file system");

    size_t payload_size = 0;
    char* payload_data = io_read_file(args->infile, 
        io_get_file_size(args->infile), 
        &payload_size
    );

    fsdriver->set_boot_sector(payload_data, 
        payload_size, &(img->partitions[0]), args->bsnoseek);
    io_write_file(args->outfile, img->image_buffer, img->image_size);
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
        case setbs:
            mode_setbs(args);
            break;
        default: fail("F: Mode not supported");
    }
    return 0;
}