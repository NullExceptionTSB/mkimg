#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <script/scriptfile.h>
#include <script/scriptparser.h>

script_op script_line_get_op(char* line) {
    if (!strncmp(line, "IMGFORMAT ", 10)) 
        return SCRIPTOP_IMGFORMAT;
    else if (!strncmp(line, "CREATE ", 7)) 
        return SCRIPTOP_CREATE;
    else if (!strncmp(line, "MKLABEL ", 8))
        return SCRIPTOP_MKLABEL;
    else if (!strncmp(line, "FORMAT ", 7)) 
        return SCRIPTOP_FORMAT;
    else if (!strncmp(line, "ADDFILE ", 9))
        return SCRIPTOP_ADDFILE;
    else if (!strncmp(line, "RMFILE ", 8))
        return SCRIPTOP_RMFILE;
    else if (!strncmp(line, "BOOTSECTOR ", 11))
        return SCRIPTOP_BOOTSECTOR;
    return SCRIPTOP_INVALID;
}

void script_freeargs(char** args, int argc) {
    for (int i = 0; i < argc; i++)
        free(args[i]);
    free(args);
}

char** script_line_getargs(char* line, int* argc) {
    int n = 0;
    char* terminator = line;
    while (terminator = strchr(terminator, ' ')) 
        n++;
    
    char** args = malloc(sizeof(char*)*n);

    terminator = line;
    n = 0;
    while (terminator = strchr(terminator, ' ')) {
        char* nextt = strchr(terminator, ' ');
        size_t sz = nextt?(nextt-terminator):strlen(terminator);

        args[n] = malloc(sz+1);
        memcpy(args[n], terminator, sz);
        args[n][sz] = 0;

        n++;
    }
    *argc = n;
    return args;
}

void script_run(scriptfile* script, FILE* outfile) {
    char* line = NULL;
    while (line = scriptfile_pull_line(script)) {


        free(line);
    }
}