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
    int size = inode_getsize(&in);
    
    int num_blocks = size / DISKIMG_SECTOR_SIZE;
    int aux = num_blocks % DISKIMG_SECTOR_SIZE;
    if (aux != 0) {
        num_blocks++;
    }
    if (blockNum < num_blocks-1){
        return 512;
    }
    if (blockNum > num_blocks-1){
        return 0;
    }
    return size % DISKIMG_SECTOR_SIZE;
}
