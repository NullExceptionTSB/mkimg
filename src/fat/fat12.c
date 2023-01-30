#include <string.h>

#include <fail.h>
#include <partition.h>

#include <fs/fat.h>
#include <fs/fat12.h>
#include <driver/fsdriver.h>

int fat12_calc_sectors_per_fat(bpb16* bpb) {
    //common value table for optimization purpouses
    switch (bpb->totalSectors) {
        case 2880: return 9;
    }

    return fat_calc_spf(12, bpb->sectorsPerCluster, 
        bpb->reservedSectors, bpb->rootDirEntries, bpb->numFats, bpb->totalSectors); //stub
}

int fat12_known_geometry_spt(int sectors) {
    switch (sectors) {
        case 2880: return 18;
    }
    return 1;
}

int fat12_known_geometry_heads(int sectors) {
    if (sectors < 10000)
        return 2;
    else return 1;
}

bpb16* fat12_write_bpb(partition* part, int small_root) {
    bpb16* bpb = malloc(sizeof(bpb16));
    memset(bpb, 0, sizeof(bpb16));

    bpb->jumpcode[0] = 0xEB;
    bpb->jumpcode[1] = sizeof(bpb16)-2;
    bpb->jumpcode[2] = 0x90;

    bpb->bytesPerSector = 512;
    memset(bpb->oem, ' ', 8);
    memcpy(bpb->oem, "mkimg", 5);
    bpb->sectorsPerCluster = 1; //forced for fat12
    bpb->reservedSectors = 1;
    bpb->numFats = 2;
    bpb->rootDirEntries = small_root ? 224 : 512; 
    bpb->totalSectors = part->partition_size/bpb->bytesPerSector;
    bpb->mediaDescriptor = 0xF8; //todo: decide media descriptor based on pars
    //todo: deprecate CHS completely
    bpb->sectorsPerTrack = fat12_known_geometry_spt(bpb->totalSectors);
    bpb->heads = fat12_known_geometry_heads(bpb->totalSectors);

    bpb->hiddenSectors = 0;
    bpb->largeSectors = 0;

    bpb->driveNum = 0;
    bpb->flags = 0;
    bpb->bootsigex = FAT_BOOT_SIG_EX;
    bpb->serial = 0x0BADBEEF;
    fat_relabel(bpb, "NOLABEL");
    memcpy(bpb->filesystem, "FAT12   ", 8);

    bpb->sectorsPerFat = fat12_calc_sectors_per_fat(bpb);

    if (bpb->sectorsPerFat == -1) 
        fail("F: Stub reached");
        
    memcpy(part->partition_buffer, bpb, sizeof(bpb16));
    return bpb;
}

void fat12_set_bootsect(char* bootsector_data, size_t bssize,
    partition* part, int seek_sector) {
    size_t seekc = sizeof(bpb16);
    if (seek_sector) 
        memcpy(part->partition_buffer + seekc, bootsector_data + seekc, 
            bssize-seekc);
    else 
        memcpy(part->partition_buffer + seekc, bootsector_data, bssize-seekc);
}

int fat12_get_next_cluster(fat12_cluster* fat, int cluster) {
    //fassert(cluster < fatsize*3/2, "F: Cluster out of range");
    fat12_cluster clust_twin = fat[cluster/2];

    if (cluster%2) 
        return (clust_twin.odd_clust1 & 0xF | (clust_twin.odd_clust2 << 4));
    else 
        return ((clust_twin.even_clust1) | clust_twin.even_clust2 << 8);
}

void fat12_set_next_cluster(fat12_cluster* fat, int cluster, int next) {
    fat12_cluster clust_twin = fat[cluster/2];
    
    if (cluster%2) {
        fat[cluster/2].odd_clust1 =   next  & 0x00F;
        fat[cluster/2].odd_clust2 =   next >> 4;
    } else {
        fat[cluster/2].even_clust1 =  next & 0x0FF;
        fat[cluster/2].even_clust2 = (next >> 8) & 0x00F;
    }
}

int fat12_allocate_clusters(fat12_cluster* fat, int num_clusters, 
    int total_clusters) {

    int allocated = 0;
    int starting = 0;
    //find first available cluster
    for (; starting < total_clusters; starting++) 
        if (!fat12_get_next_cluster(fat, starting)) 
            break;
    int previous = starting;
    int eof = 0;
    //allocate
    while (allocated < num_clusters) {
        //find next free cluster
        int next = previous + 1;
        while (fat12_get_next_cluster(fat, next)){
            fassert(next < total_clusters, "F: File too large");
            next++;
        } 
        fat12_set_next_cluster(fat, previous, next);
        eof = previous;
        previous = next;
        allocated++;
    }
    fat12_set_next_cluster(fat, eof, 0xFFF);
    return starting;
}

void fat12_write_cluster_chain(int start_cluster, char* data_area,
    char* data, fat12_cluster* fat, size_t data_size, int bytes_per_cluster){
    size_t remaining = data_size;
    int cluster = start_cluster;
    while (remaining) {
        if (remaining < bytes_per_cluster) {
            memcpy(
                data_area+(cluster-2)*bytes_per_cluster, 
                data+(data_size-remaining),
                remaining
            );
            break;
        }
        memcpy(
            data_area+(cluster-2)*bytes_per_cluster,
            data+(data_size-remaining),
            bytes_per_cluster
        );
        remaining -= bytes_per_cluster;
        cluster = fat12_get_next_cluster(fat, cluster);
    }
}

void fat12_insert_rootdir_entry(rootdir_entry* root_directory, 
    rootdir_entry* entry, int entry_count) {
    int free_index = -1;
    for (int i = 0; i < entry_count; i++) 
        if (!root_directory[i].filename[0]) {
            free_index = i;
            break;
        }

    fassert(free_index != -1, "F: Too many files");

    memcpy(root_directory+free_index, entry, sizeof(rootdir_entry));
}

void fat12_add_file(char* filename, char* data, 
    size_t data_size, partition* part) {
    bpb16* bpb = (bpb16*)part->partition_buffer;

    char* pfat = (char*)(part->partition_buffer + 
        bpb->reservedSectors * bpb->bytesPerSector);
    char* prootdir = pfat + 
        bpb->numFats * bpb->sectorsPerFat * bpb->bytesPerSector;
    char* pdata = prootdir + bpb->rootDirEntries*32; 

    int cluster_count = data_size / 
        (bpb->sectorsPerCluster*bpb->bytesPerSector);
    if (data_size % (bpb->sectorsPerCluster*bpb->bytesPerSector)) 
        cluster_count++;
    
    int start_cluster = fat12_allocate_clusters(
        (fat12_cluster*)pfat, 
        cluster_count,
        ((bpb->sectorsPerFat*bpb->bytesPerSector)/3)*2
    );

    fassert(start_cluster, "F: Drive full");
    
    fat12_write_cluster_chain(start_cluster, pdata, 
        data, (fat12_cluster*)pfat, data_size, 
        bpb->sectorsPerCluster * bpb->bytesPerSector);

    fat_sync_fats(part, 0);

    rootdir_entry* entry = malloc(sizeof(rootdir_entry));
    memset(entry, 0, sizeof(rootdir_entry));
    char* sfn = fat_new_short_filename(filename);

    memcpy(entry->filename, sfn, 11);
    entry->file_size = data_size;
    entry->start_cluster = start_cluster;
    fat12_insert_rootdir_entry((rootdir_entry*)prootdir, 
        entry, bpb->rootDirEntries);
}

void fat12_make_tables(partition* part) {
    bpb16* bpb = (bpb16*)part->partition_buffer;
    int sect_sz = bpb->bytesPerSector;

    void* table_start = part->partition_buffer + bpb->reservedSectors*sect_sz;
    int max_clusters = ((bpb->sectorsPerFat*sect_sz)/3)*2;

    for (int t = 0; t < bpb->numFats; t++) {
        char* ctable = (char*)(table_start + sect_sz*bpb->sectorsPerFat*t);
        ctable[0] = 0xF0;
        ctable[1] = 0xFF;
        ctable[2] = 0xFF;

        fat12_cluster* fat = (fat12_cluster*)ctable;

        for (int i = 2; i < max_clusters; i++) 
            fat12_set_next_cluster(fat, i, 0);
    }
}

void fat12_format(partition* part, mkimg_args* args) {
    fassert(args->create_sz_lba >= 7, 
        "F: Partition too small for selected filesystem");
    
    bpb16* bpb = fat12_write_bpb(part, FAT12_FORCE_SMALL_ROOT);
    fat12_make_tables(part);
    fat_putdir(part->partition_buffer + 
        bpb->reservedSectors*bpb->bytesPerSector+
        bpb->sectorsPerFat*bpb->numFats*bpb->bytesPerSector,
        bpb->rootDirEntries);
}

const mkimg_fsdriver fsdriver_fat12 = 
    {"FAT12", FSFAT12, 
        fat12_add_file, 
        fat12_format, 
        fat12_set_bootsect };