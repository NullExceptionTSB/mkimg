CC = gcc -c
LD = gcc
CCARGS = -s -std=c99 -Iinclude
LNARGS = -s -O3
EXECNAME = mkimg

all:
	$(CC) $(CCARGS) src/main.c -o build/main.o
	$(CC) $(CCARGS) src/io.c -o build/io.o
	$(CC) $(CCARGS) src/image.c -o build/image.o
	$(CC) $(CCARGS) src/chs.c -o build/chs.o
	$(CC) $(CCARGS) src/arg.c -o build/arg.o
	$(CC) $(CCARGS) src/create/mbr.c -o build/create_mbr.o
	$(LD) -lm build/*.o -o $(EXECNAME)

stub: 
	rm -rf mbr
	mkdir mbr
	nasm asm/bldr_stub_mbr.asm -o mbr/mbrstub.bin
