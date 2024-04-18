#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <script/scriptfile.h>

scriptfile* scriptfile_new(char* path) {
    scriptfile* script = malloc(sizeof(scriptfile));
    if (!script) return NULL;

    script->file = fopen(path, "r");
    if (!script->file) goto fail;

    script->line = 0;
    return script;

    fail:
    free(script);
    return NULL;
}

char* scriptfile_pull_line(scriptfile* sf) {
    char buff[1024];
    memset(buff, 0, 1024);

    size_t ammt = fread(buff, 1, 1023, sf->file);
    char* lineend = strchr(buff, '\n');    
    if (!lineend) return NULL;
    
    size_t linesz = lineend - buff;

    char* ret = malloc(linesz+1);
    if (!ret) return NULL;

    fseek(sf->file, linesz-ammt+1, SEEK_CUR);

    memcpy(ret, buff, linesz);
    ret[linesz] = 0;
    sf->line++;
    return ret;
}