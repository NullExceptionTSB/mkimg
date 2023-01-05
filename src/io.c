#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* io_read_file(char* path, size_t read_ammount, size_t* actually_read) {
    if (!read_ammount) return NULL;
    FILE* file = fopen(path, "rb");
    if (!file) return NULL;

    void* buffer = malloc(read_ammount);
    if (!buffer) goto fail;

    size_t read_ammt = fread(buffer, 1, read_ammount, file);

    fail:
    if (file) fclose(file);
    if (actually_read) *actually_read = read_ammt;
    return buffer;
}

size_t io_get_file_size(char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) return 0;
    fseek(file, 0, SEEK_END);

    size_t filesz = ftell(file);
    fclose(file);
    return filesz;
}

size_t io_write_file(char* path, char* data, size_t sz_data) {
    FILE* file = fopen(path, "wb");
    if (!file) return 0;

    size_t written = fwrite(data, 1, sz_data, file);

    fail:
    if (file) fclose(file);
    return written;
}