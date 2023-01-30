#include <stdlib.h>
#include <stdio.h>

#include <fail.h>

void fail(char* msg) {
    puts(msg);
    exit(-1);
}
void fassert(int condition, char* msg) {
    if (!condition) fail(msg);
}