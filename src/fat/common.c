#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <fs/fat.h>

void fat_relabel(bpb16* bpb, char* newlabel) {
    //strnlen is not in C99
    int len = strlen(newlabel);
    len = len>11?11:len;

    memcpy(bpb->label, newlabel, len);
}

char* fat_new_short_filename(char* long_filename) {
    char* sfn = malloc(11);
    memset(sfn, ' ', 11);

    //calculate length of name portion
    size_t fn_len, ex_len;
    for (fn_len = 0; fn_len < strlen(long_filename); fn_len++) 
        if (long_filename[fn_len]=='.') break;
    ex_len = strlen(long_filename) - fn_len - 1;
    
    //take max first 8 chars of filename and 3 chars of extension

    fn_len = (8>=fn_len?fn_len:8);
    ex_len = (3>=ex_len?ex_len:3);

    for (int i = 0; i < fn_len; i++) 
        sfn[i] = toupper(long_filename[i]);
    
    for (int i = 0; i < ex_len; i++)
        sfn[8+i] = toupper(long_filename[fn_len+1+i]);
    return sfn;
}

void fat_putdir(char* ptr, int entries) {
    memset(ptr, 0, entries*32);
}