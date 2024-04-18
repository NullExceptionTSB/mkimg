#pragma once
#include <stdio.h>

#include <image.h>

typedef struct _MKIMG_SCRIPT {
    FILE* file;
    int line;

    image* image;
}scriptfile;

scriptfile* scriptfile_new(char* path) ;
char* scriptfile_pull_line(scriptfile* sf);