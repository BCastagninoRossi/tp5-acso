#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode in;
    if (inode_iget(fs, inumber, &in) < 0) {
        return -1;
    }
    int64_t size = inode_getsize(&in);

    int16_t real_block_num = inode_indexlookup(fs, &in, blockNum);
    if (real_block_num < 0){
        return -1;   
    }
    
    if (diskimg_readsector(fs->dfd, real_block_num, buf) <0) {
        return -1;
    }

    int64_t num_blocks = size / DISKIMG_SECTOR_SIZE;
    int64_t aux = num_blocks % DISKIMG_SECTOR_SIZE;
    if (aux != 0) {
        num_blocks++;
    }
    if (blockNum < num_blocks-1){
        return DISKIMG_SECTOR_SIZE;
    }
    if (blockNum > num_blocks-1){
        return 0;
    }
    return size % DISKIMG_SECTOR_SIZE;
}
