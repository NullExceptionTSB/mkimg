#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <arg.h>
#include <fail.h>
#include <fat.h>
#include <mbr.h>
#include <image.h>

int fat12_calc_sectors_per_fat(bpb16* bpb) {
    //generic values
    switch (bpb->totalSectors) {
        case 2880: return 9;
        //todo: expand
    }

    //calculate if undefined
    return -1; //stub
}

void fat12_relabel(bpb16* bpb, char* newlabel) {
    //strnlen is not in C99
    int len = strlen(newlabel);
    len = len>11?11:len;

    memcpy(bpb->label, newlabel, len);
}

bpb16* fat12_write_bpb_direct(image* img) {
    bpb16* bpb = calloc(1, sizeof(bpb16));
    bpb->bytesPerSector = 512;
    memset(bpb->oem, ' ', 8);
    bpb->sectorsPerCluster = 1; //forced for fat12
    bpb->reservedSectors = 1;
    bpb->numFats = 2;
    //FLOPPY ONLY! HARD DRIVES USE 512
    bpb->rootDirEntries = img->image_template ? 224 : 512; 
    bpb->totalSectors = img->image_size/bpb->bytesPerSector;
    bpb->mediaDescriptor = 0xF0; //FLOPPY ONLY!
    

    bpb->sectorsPerTrack = img->image_size_chs.spt;
    bpb->heads = img->image_size_chs.head;
    bpb->hiddenSectors = 0;
    bpb->largeSectors = 0;

    bpb->driveNum = 0;
    bpb->flags = 0;
    bpb->bootsigex = FAT_BOOT_SIG_EX;
    bpb->serial = 0x0BADBEEF;
    fat12_relabel(bpb, "NOLABEL");
    memcpy(bpb->filesystem, "FAT12   ", 8);

    bpb->sectorsPerFat = fat12_calc_sectors_per_fat(bpb);

    if (bpb->sectorsPerFat == -1) 
        fail("F: Stub reached");
        
    memcpy(img->image_buffer, bpb, sizeof(bpb16));
    return bpb;
}

void fat12_puttables(char* ptr, int table_size_sectors, int num_tables) {
    memset(ptr, 0, table_size_sectors*512*num_tables);
    for (int i = 0; i < num_tables; i++) {
        (ptr+(table_size_sectors*512*i))[0] = 0xF0;
        (ptr+(table_size_sectors*512*i))[1] = 0xFF;
        (ptr+(table_size_sectors*512*i))[2] = 0xFF;   
    }
}

void fat12_putroot(char* ptr, int entries) {
    memset(ptr, 0, entries*32);
}

int fat12_get_chain_next(fat12_cluster* fat, int current_cluster) {
    return current_cluster % 2 ? 
            fat[current_cluster/2].even_clust1 | fat[current_cluster/2].odd_clust2 << 8:
            fat[current_cluster/2].even_clust1 << 4 | fat[current_cluster/2].even_clust2;
}

void fat12_freechain(image* img, bpb16* bpb, int start_cluster_idx) {
    fat12_cluster* fat = img->image_buffer + bpb->reservedSectors;
    int clust_next = start_cluster_idx;
    do {
        fat12_cluster twinc = fat[clust_next/2];
        int cluster = fat12_get_chain_next(fat, clust_next);

        if (clust_next%2) {
            fat[clust_next/2].odd_clust1 = 0;
            fat[clust_next/2].odd_clust2 = 0;
        } else {
            fat[clust_next/2].even_clust1 = 0;
            fat[clust_next/2].even_clust2 = 0;
        }

    } while (clust_next < 0xFF8);
}
//returns: starting cluster #
int fat12_allocclusters(image* img, bpb16* bpb, int cluster_count) { 
    fat12_cluster* fat_start = img->image_buffer + bpb->reservedSectors;
    int clusters_allocated = 0, prev_al_clus = 0, first_clus = 0;
    for (int i = 2; i < bpb->sectorsPerFat*512*2/3; i++) {
        int cluster = i%2
            ?fat_start[i/2].odd_clust1 | fat_start[i/2].odd_clust2 << 8
            :fat_start[i/2].even_clust1 << 4 | fat_start[i/2].even_clust2;
        

        if (!cluster) {
            if (prev_al_clus) {
                if (prev_al_clus % 2) {
                    fat_start[prev_al_clus/2].odd_clust1 = (char)(i&0xF);
                    fat_start[prev_al_clus/2].odd_clust2 = (char)(i>>8);
                }
                else {
                    fat_start[prev_al_clus/2].even_clust1 = (char)i;
                    fat_start[prev_al_clus/2].even_clust2 = (char)((i>>8)&0xF);
                }
            } else first_clus = cluster;
            prev_al_clus = cluster;
            clusters_allocated++;

            if (cluster_count == clusters_allocated) {
                if (i%2) {
                    fat_start[i/2].odd_clust1 = 0xF;
                    fat_start[i/2].odd_clust2 = 0xF8;
                } else {
                    fat_start[i/2].even_clust1 = 0xFF;
                    fat_start[i/2].even_clust2 = 0x8;
                }
                break;
            }
        }
    }
    if (clusters_allocated == 0)
        fail("F: Image is full");
        
    if (cluster_count < clusters_allocated) {
        fat12_freechain(img, bpb, first_clus);
        return -1;
    }
    return first_clus;
}

void fat12_write_file_via_cluster_chain(char* data, size_t data_size, 
    image* img, bpb16* bpb, int start_cluster_idx) {

    size_t data_left = data_size;
    int spc = bpb->sectorsPerCluster;
    fat12_cluster* fat = img->image_buffer + bpb->reservedSectors;
    char* data_area = img->image_buffer + ((bpb->reservedSectors + 
        bpb->sectorsPerFat * bpb->numFats) * bpb->bytesPerSector + 
        32 * bpb->rootDirEntries);

    int clustern = start_cluster_idx;
    do {
        //go foolish yes
        size_t copy_size = 
            data_left > bpb->sectorsPerCluster*bpb->bytesPerSector?
            bpb->sectorsPerCluster*bpb->bytesPerSector : data_left;        
        memcpy(data_area + 
            (clustern-2) * bpb->sectorsPerCluster * bpb->bytesPerSector,
            data + (data_size - data_left),
            copy_size);
        data_left -= copy_size;
    } while (data_left);
}

char* fat12_new_short_filename(char* long_filename) {
    char* sfn = malloc(11);
    memset(sfn, ' ', 11);

    //calculate length of name portion
    size_t fn_len, ex_len;
    for (fn_len = 0; fn_len < strlen(long_filename); fn_len++) 
        if (long_filename[fn_len]=='.') break;
    ex_len = strlen(long_filename) - fn_len;

    //take max first 8 chars of filename and 3 chars of extension
    for (int i = 0; i < 8>fn_len?fn_len:8; i++) 
        sfn[i] = toupper(long_filename[i]);
    for (int i = 0; i < 3>ex_len?ex_len:3; i++)
        sfn[8+i] = toupper(long_filename[fn_len+1+i]);

    return sfn;
    
}

void fat12_addfile(char* filename, char* data, size_t data_size, image* img) {
    if (data_size > img->image_size) 
        fail("F: File larger than entire image !");

    bpb16* bpb_16;
    switch (img->image_partition_table) {
        case PARTTYPE_NONE:
            bpb_16 = img->image_buffer;
            
            int clusters_needed = data_size / 
                (bpb_16->sectorsPerCluster * bpb_16->bytesPerSector);
            
            if (clusters_needed > 4094) //2^24-2
                fail("F: File too big for this filesystem");
            else if (clusters_needed > bpb_16->sectorsPerFat*512*2/3)
                fail("F: File too big for this volume");
            
            int startc = fat12_allocclusters(img, bpb_16, clusters_needed);
            if (startc == -1)
                fail("F: Not enough space for file");

            fat12_write_file_via_cluster_chain(data, data_size, img,
                bpb_16, startc);

            rootdir_entry* rootdir = bpb_16->hiddenSectors + 
                bpb_16->numFats + bpb_16->sectorsPerFat * 
                bpb_16->bytesPerSector;
            int free_entry_found = 0;
            for (int i = 0; i < bpb_16->rootDirEntries; i++) {
                if (rootdir[i].filename[0] == '\0') {
                    free_entry_found = 1;
                    char* sfn = fat12_new_short_filename(filename);
                    memcpy(rootdir+i, sfn, 11); //rootdir+i actually means
                                                //rootdir+(sizeof(entry)*i)
                    free(sfn);
                    rootdir[i].start_cluster = startc;
                    break;
                }
            }

            if (!free_entry_found)
                fail("F: Too many files on drive");
            break;
        default:
            fail("F: Unsupported partition type, cannot continue");
            return;
    }
}

void fat12_format(image* img) {
    if (img->image_size < 7*512) //minimum size for FAT12 is 7 sectors
        fail("F: Image too small");

    bpb16* bpb_16;
    switch (img->image_partition_table) {
        case PARTTYPE_NONE:
            bpb_16 = fat12_write_bpb_direct(img);
            fat12_puttables(img->image_buffer + 
                bpb_16->reservedSectors*bpb_16->bytesPerSector,
                bpb_16->sectorsPerFat, bpb_16->numFats);
            fat12_putroot(img->image_buffer + 
                bpb_16->reservedSectors*bpb_16->bytesPerSector+
                bpb_16->sectorsPerFat*bpb_16->numFats*bpb_16->bytesPerSector,
                bpb_16->rootDirEntries);
            break;
        default:
            puts("F: Unsupported partition type, cannot format");
            return;
    }
}