#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"


/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (inumber < 1 || inumber > fs->superblock.s_fsize) return -1;
    int inode_size = sizeof(struct inode);
    int inodes_per_block = DISKIMG_SECTOR_SIZE / inode_size;
    int16_t block = (inumber - 1) / inodes_per_block + INODE_START_SECTOR;
    int16_t offset = (inumber - 1) % inodes_per_block;
    struct inode *inodes = malloc(DISKIMG_SECTOR_SIZE);
    if (inodes == NULL) return -1;
    if (diskimg_readsector(fs->dfd, block, inodes) == -1) {
        free(inodes);
        return -1;
    }
    *inp = inodes[offset];
    free(inodes);
    return 0;
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {  
    //Implement code here
    int inode_file_size = inode_getsize(inp);
    if (inode_file_size == 0) return -1;
    int inode_blocks = inode_file_size/DISKIMG_SECTOR_SIZE;
    if (blockNum < 0 || blockNum >= inode_blocks) return -1;
    if (inode_file_size <= DISKIMG_SECTOR_SIZE*8){
        return inp->i_addr[blockNum];
    }
    int indir_block_num = blockNum / 256;
    int offset = blockNum % 256;
    int *indir_block = malloc(DISKIMG_SECTOR_SIZE);
    if (indir_block_num < 7){
        if (diskimg_readsector(fs->dfd, inp->i_addr[indir_block_num], indir_block) == -1) {
        free(indir_block);
        return -1;
        } else {
            int block = indir_block[offset];
            free(indir_block);
            return block;
        }
    }
    if (indir_block_num >= 7){
        int indir_block_num = (blockNum - 7*256) / 256;
        int offset = (blockNum - 7*256) % 256;
        if (diskimg_readsector(fs->dfd, inp->i_addr[7], indir_block) == -1) {
            free(indir_block);
        return -1;
        } else {
            int *second_indir_block = malloc(DISKIMG_SECTOR_SIZE);
            if (diskimg_readsector(fs->dfd, indir_block[indir_block_num], second_indir_block) == -1) {
                free(second_indir_block);
                free(indir_block);
                return -1;
            }
            int block = second_indir_block[offset];
            free(indir_block);
            free(second_indir_block);
            return block;
            }
    }
    return 0;
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
