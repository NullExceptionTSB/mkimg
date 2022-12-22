CC = gcc -c
LD = gcc
CCARGS = -s -std=c99 -Iinclude
LNARGS = -s -O3
EXECNAME = mkimg

all: build mbr
	$(CC) $(CCARGS) src/main.c -o build/main.o
	$(CC) $(CCARGS) src/io.c -o build/io.o
	$(CC) $(CCARGS) src/image.c -o build/image.o
	$(CC) $(CCARGS) src/chs.c -o build/chs.o
	$(CC) $(CCARGS) src/arg.c -o build/arg.o
	$(CC) $(CCARGS) src/create/mbr.c -o build/create_mbr.o
	$(CC) $(CCARGS) src/fat/fat12.c -o build/fat_fat12.o
	$(LD) -lm build/*.o -o $(EXECNAME)

mbr:
	-@mkdir mbr
	nasm asm/bldr_stub_mbr.asm -o mbr/mbrstub.bin

build:
	-@mkdir build

install:
	@echo Install currently not supported

.PHONY: clean all
clean:
	rm -rf build
	rm -rf mbr
	rm mkimg