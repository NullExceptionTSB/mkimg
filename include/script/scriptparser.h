#pragma once
#include <stdio.h>
#include <script/scriptfile.h>

typedef enum _MKIMG_SCRIPT_OPERATION {
    SCRIPTOP_INVALID
    SCRIPTOP_IMGFORMAT,
    SCRIPTOP_CREATE,
    SCRIPTOP_MKLABEL,
    SCRIPTOP_FORMAT,
    SCRIPTOP_ADDFILE,
    SCRIPTOP_RMFILE,
    SCRIPTOP_BOOTSECTOR
}script_op;

void script_run(scriptfile* script, FILE* outfile);