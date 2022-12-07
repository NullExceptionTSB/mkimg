#pragma once
#include <stdlib.h>

char* io_read_file(char* path, size_t read_ammount, size_t* actually_read);
size_t io_get_file_size(char* path);
size_t io_write_file(char* path, char* data, size_t sz_data);