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
    {"cylinders", 'C', "cylinders", 0, "Cylinder count, incompatible with -s, -t", 1},
    {"heads", 'H', "heads", 0, "Head count, incompatible with -s, -t", 1},
    {"sectors", 'S', "sectors", 0, "Sectors per cylinder, incompatible with -s, -t", 1},

    {"size", 's', "size", 0, "LBA in sectors, incompatible with -C, -H, -S, -t", 2},

    {"out", 'o', "outfile", 0, "Output image file, mandatory in create mode", 3},
    {"in", 'i', "infile", 0, "Input image file, mandatory for add mode. In create mode, specifies stage 1 bootloader binary file.", 3},

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
            pargs->create_sz_cylinders = 2880/18;
            pargs->create_sz_spt = 18;
            pargs->create_sz_lba = 2880*512;
            pargs->create_sizemode = CHS;
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
            if (!pargs->mode != MODE_UNDECIDED && !pargs->mode != create) 
                puts("W: Mode set twice (assuming first set is desired)");
            
            pargs->mode = create;
            break;
        case 'a':
            puts("F: Not supported");
            break;
        case 't':
            pargs->create_template = 1;
            if (!arg_process_template(pargs, strtol(arg, NULL, 10)))
                arg_fail("F: Invalid template");
            break;
        case 'C':
            if (pargs->create_template)
                arg_fail("F: Cannot specify -t with other size paramteres");
            arg_delimit_sizemode(CHS, pargs, state);
            pargs->create_sz_cylinders = strtol(arg, NULL, 10);
            break;
        case 'H':
            if (pargs->create_template)
                arg_fail("F: Cannot specify -t with other size paramteres");
            arg_delimit_sizemode(CHS, pargs, state);
            pargs->create_sz_heads = strtol(arg, NULL, 10);
            break;
        case 'S':
            if (pargs->create_template)
                arg_fail("F: Cannot specify -t with other size paramteres");
            arg_delimit_sizemode(CHS, pargs, state);
            pargs->create_sz_spt = strtol(arg, NULL, 10);
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
                    break;
            }
            if (!args->outfile)
                arg_fail("F: No output file specified for create mode");
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
    arg_lint(args);

    free(argp_struct);
    return args;
}