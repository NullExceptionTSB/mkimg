CC = gcc -c
LD = gcc
CCARGS = -s -std=c99 -Iinclude
LNARGS = -s -O3
EXECNAME = mkimg

all: build mbr $(EXECNAME)
	
$(EXECNAME): build/arg.o build/chs.o build/defaults.o build/fail.o build/filesystem.o build/image.o build/io.o build/main.o build/partition.o build/part_mbr.o build/part_unpart.o build/fat12.o build/fat_common.o build/script_file.o build/script_main.o build/script_parser.o
	$(LD) build/*.o -lm -o $(EXECNAME)

build/arg.o: src/arg.c
	$(CC) $(CCARGS) src/arg.c -o build/arg.o
build/chs.o: src/chs.c
	$(CC) $(CCARGS) src/chs.c -o build/chs.o
build/defaults.o: src/defaults.c
	$(CC) $(CCARGS) src/defaults.c -o build/defaults.o
build/fail.o: src/fail.c
	$(CC) $(CCARGS) src/fail.c -o build/fail.o
build/filesystem.o: src/filesystem.c
	$(CC) $(CCARGS) src/filesystem.c -o build/filesystem.o
build/image.o: src/image.c
	$(CC) $(CCARGS) src/image.c -o build/image.o
build/io.o: src/io.c
	$(CC) $(CCARGS) src/io.c -o build/io.o
build/main.o: src/main.c
	$(CC) $(CCARGS) src/main.c -o build/main.o
build/partition.o: src/partition.c
	$(CC) $(CCARGS) src/partition.c -o build/partition.o	

build/part_mbr.o: src/partition_table/mbr.c
	$(CC) $(CCARGS) src/partition_table/mbr.c -o build/part_mbr.o
build/part_unpart.o: src/partition_table/unpart.c
	$(CC) $(CCARGS) src/partition_table/unpart.c -o build/part_unpart.o

build/script_main.o: src/script/scriptmain.c
	$(CC) $(CCARGS) src/script/scriptmain.c -o build/script_main.o
build/script_file.o: src/script/scriptfile.c
	$(CC) $(CCARGS) src/script/scriptfile.c -o build/script_file.o
build/script_parser.o: src/script/scriptparser.c
	$(CC) $(CCARGS) src/script/scriptparser.c -o build/script_parser.o

build/fat_common.o: src/fat/common.c 
	$(CC) $(CCARGS) src/fat/common.c -o build/fat_common.o
build/fat12.o: src/fat/fat12.c 
	$(CC) $(CCARGS) src/fat/fat12.c -o build/fat12.o

mbr:
	-@mkdir mbr
	nasm asm/bldr_stub_mbr.asm -o mbr/mbrstub.bin

build:
	-@mkdir build

install:
	cp -f mkimg /usr/bin/mkimg
	-@mkdir /usr/share/mkimg 2>&1 > /dev/null | true
	cp -fr mbr /usr/share/mkimg

.PHONY: clean all
clean:
	rm -rf build
	rm -rf mbr
	rm -f mkimg