#include <io.h>

char* defaults_get_bootsect_stub() {
    char* bootsect_stub = 
        io_read_file("mbr/mbrstub.bin", 512, NULL);
    //try the other path one
    if (!bootsect_stub)
        bootsect_stub =
            io_read_file("/usr/share/mkimg/mbr/mbrstub.bin", 
                512, NULL);

    if (!bootsect_stub)
        return NULL;
}