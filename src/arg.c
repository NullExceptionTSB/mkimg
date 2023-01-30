#include <argp.h>
#include <arg.h>
#include <stdlib.h>
#include <string.h>

static struct argp_option options[] = {
    //{"help", 'h', 0, 0, "Shows this help screen"},
    {"verbose", 'v', 0, 0, "Verbose mode", -1},

    {"create", 'c', 0, 0, "Creates a new image", 0},
    {"add", 'a', 0, 0, "Adds a file", 0},

    {"template", 't', "template-num", 0, "Create using template (see man mkimg)"},
    {"cylinders", 'C', "cylinders", 0, "(deprec.) Cylinder count, incompatible with -s, -t", 1},
    {"heads", 'H', "heads", 0, "(deprec.) Head count, incompatible with -s, -t", 1},
    {"sectors", 'S', "sectors", 0, "(deprec.) Sectors per cylinder, incompatible with -s, -t", 1},

    {"size", 's', "size", 0, "LBA in sectors, incompatible with -C, -H, -S, -t", 2},

    {"out", 'o', "outfile", 0, "Output image file. In add mode, specifies image file.", 3},
    {"in", 'i', "infile", 0, "Input image file, mandatory for add mode. In create mode, specifies stage 1 bootloader binary file.", 3},

    {"filesystem", 'f', "filesystem", 0, "Create mode: desired filesystem to format the image with. Add mode: explicitly stated filesystem (skip autodetect)", 4},
    {"ptformat", 'p', "partition-table-format", 0, "Create mode: desired partition table to use for image (default: NONE). Add mode: explicitly stated partition table (skip autodetect)"},
    
    {"nobsseek", 'N', 0, 0, "Don't seek over BPB in bootsector files"},

    {0}
};

static const char doc[] = "mkimg - Storage media image generator, intended for OS development";

void arg_delimit_sizemode(mkimg_sizemode sizemode, mkimg_args* pargs, struct argp_state* state) {
    if (pargs->create_sizemode != sizemode && pargs->create_sizemode != SIZEMODE_UNDECIDED) 
        argp_usage(state);
    else
        pargs->create_sizemode = sizemode;
}

int arg_process_template(mkimg_args* pargs, int template) {
    switch (template) {
        case 1:
            pargs->create_sz_heads = 2;
            pargs->create_sz_cylinders = 1440/18;
            pargs->create_sz_spt = 18;
            pargs->create_sz_lba = 2880*512;
            pargs->create_sizemode = CHS;
            pargs->create_desiredfs = FSFAT12;
            pargs->partition_type = PARTTYPE_NONE;
            return 1;
        default: return 0;
    }
}

static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    mkimg_args* pargs = state->input;
    
    switch (key) {
        case 'v':
            pargs->verbose = 1;
            break;
        case 'h':   
            argp_state_help(state, stdout, 0);
            break;
        case 'c':
            if ((pargs->mode != MODE_UNDECIDED) && (pargs->mode != create)) 
                puts("W: Mode set twice (assuming first set is desired)");
            
            pargs->mode = create;
            break;
        case 'a':
            if ((pargs->mode != MODE_UNDECIDED) && (pargs->mode != cpfile)) 
                puts("W: Mode set twice (assuming first set is desired)");

            pargs->mode = cpfile;
            break;
        case 't':
            pargs->create_template = 1;
            if (!arg_process_template(pargs, strtol(arg, NULL, 10)))
                arg_fail("F: Invalid template");
            break;
        case 'C':
            puts("W: Deprecated option -C used");
            if (pargs->create_template)
                arg_fail("F: Cannot specify -t with other size paramteres");
            arg_delimit_sizemode(CHS, pargs, state);
            pargs->create_sz_cylinders = strtol(arg, NULL, 10);
            break;
        case 'H':
            puts("W: Deprecated option -H used");
            if (pargs->create_template)
                arg_fail("F: Cannot specify -t with other size paramteres");
            arg_delimit_sizemode(CHS, pargs, state);
            pargs->create_sz_heads = strtol(arg, NULL, 10);
            break;
        case 'S':
            puts("W: Deprecated option -S used");
            if (pargs->create_template)
                arg_fail("F: Cannot specify -t with other size paramteres");
            arg_delimit_sizemode(CHS, pargs, state);
            pargs->create_sz_spt = strtol(arg, NULL, 10);
            break;
        case 'f':
            if (!strcmp(arg, "FAT12"))
                pargs->create_desiredfs = FSFAT12;
            else if (!strcmp(arg, "FAT16"))
                pargs->create_desiredfs = FSFAT16;
            else if (!strcmp(arg, "FAT32"))
                pargs->create_desiredfs = FSFAT32;
            else arg_fail("F: Invalid or unsupported filesystem");
            break;
        case 'p':
            if (!strcmp(arg, "NONE")) 
                pargs->partition_type = PARTTYPE_NONE;
            else if (!strcmp(arg, "MBR"))
                pargs->partition_type = PARTTYPE_MBR;
            else if (!strcmp(arg, "GPT"))
                pargs->partition_type = PARTTYPE_GPT;
            break;
        case 's':
            if (pargs->create_template)
                arg_fail("F: Cannot specify -t with other size paramteres");
            arg_delimit_sizemode(LBA, pargs, state);
            pargs->create_sz_lba = strtol(arg, NULL, 10);
            break;
        case 'o':
            pargs->outfile = malloc(strlen(arg)+1);
            strcpy(pargs->outfile, arg);
            break;
        case 'i':
            pargs->infile = malloc(strlen(arg)+1);
            strcpy(pargs->infile, arg);
            break;
        case 'N':
            pargs->bsnoseek = 1;
            break;
        case ARGP_KEY_END:
            if (pargs->mode == MODE_UNDECIDED)
                argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

void arg_fail(char* msg) {
    puts(msg);
    exit(-1);
}

void arg_lint(mkimg_args* args) {
    switch(args->mode) {
        case create:
            switch(args->create_sizemode) {
                case SIZEMODE_UNDECIDED:
                    arg_fail("F: Size not specified");
                case CHS:
                    if (args->create_sz_cylinders <= 0 || 
                        args->create_sz_heads <= 0 ||
                        args->create_sz_spt <= 0)
                        arg_fail("F: Incomplete or invalid CHS size specified");
                    break;
                case LBA:
                    if (args->create_sz_lba <= 0) 
                        arg_fail("F: Invalid size specified");
                    else if (args->create_sz_lba % 512)
                        arg_fail("F: Disk size is not a multiple of 1 sector (512 bytes)");
                    break;
            }
            if (!args->partition_type)
                args->partition_type = PARTTYPE_NONE;

            if (!args->outfile)
                arg_fail("F: No output file specified for create mode");
            break;
        case cpfile:
            if(!args->outfile)
                arg_fail("F: No image file specified for add mode");
            if (!args->infile)
                arg_fail("F: No input file specified for add mode");
            break;
        default:
            arg_fail("F: Specified mode not supported or invalid");            
    }
}

mkimg_args* arg_parse(int argc, char* argv[]) {
    struct argp* argp_struct = malloc(sizeof(struct argp));
    mkimg_args* args = malloc(sizeof(mkimg_args));
    
    memset(args, 0, sizeof(mkimg_args));
    memset(argp_struct, 0, sizeof(struct argp));
    argp_struct->options = options;
    argp_struct->doc = doc;
    argp_struct->parser = parse_opt;

    error_t parsecode = argp_parse(argp_struct, argc, argv, 0, NULL, args);
    args->create_sz_lba *= 512;
    arg_lint(args);

    free(argp_struct);
    
    return args;
}