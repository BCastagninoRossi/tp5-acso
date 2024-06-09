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
    int block = (inumber - 1) / inodes_per_block + INODE_START_SECTOR;
    int offset = (inumber - 1) % inodes_per_block;
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
// int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {  
//     int inode_file_size = inode_getsize(inp);
//     //if (inode_file_size == 0) return -1;

//     if (inode_file_size <= DISKIMG_SECTOR_SIZE*8){
//         return inp->i_addr[blockNum];
//     }

//     int inode_blocks = inode_file_size/DISKIMG_SECTOR_SIZE;
//     int aux = inode_file_size % DISKIMG_SECTOR_SIZE;
//     if (aux != 0) {
//         inode_blocks++;
//     }
//     if (blockNum < 0 || blockNum >= inode_blocks) return -1;



//     int16_t indir_block_num = blockNum / 256;
//     int16_t offset = blockNum % 256;
//     int16_t *indir_block = malloc(DISKIMG_SECTOR_SIZE);
//     if (indir_block_num < 7){
//         if (diskimg_readsector(fs->dfd, inp->i_addr[indir_block_num], indir_block) == -1) {
//         free(indir_block);
//         return -1;
//         } else {
//             int16_t block = indir_block[offset];
//             free(indir_block);
//             return block;
//         }
//     }
//     if (indir_block_num >= 7){
//         int16_t indir_block_num = (blockNum - 7*256) / 256;
//         int16_t offset = (blockNum - 7*256) % 256;
//         if (diskimg_readsector(fs->dfd, inp->i_addr[7], indir_block) == -1) {
//             free(indir_block);
//         return -1;
//         } else {
//             int16_t *second_indir_block = malloc(DISKIMG_SECTOR_SIZE);
//             if (diskimg_readsector(fs->dfd, indir_block[indir_block_num], second_indir_block) == -1) {
//                 free(second_indir_block);
//                 free(indir_block);
//                 return -1;
//             }
//             int16_t block = second_indir_block[offset];
//             free(indir_block);
//             free(second_indir_block);
//             return block;
//             }
//     }
//     return 0;
// }

/////////////////
///////////////

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
	int fd = fs->dfd;
	int is_small_file = ((inp->i_mode & ILARG) == 0);

	// if it is a small file
	if(is_small_file) {
		return inp->i_addr[blockNum];
	}	

	// if it is a large file
	int addr_num = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);
	int indir_addr_num = addr_num * INDIR_ADDR;
	if(blockNum < indir_addr_num) {		// if it only uses INDIR_ADDR
		int sector_offset = blockNum / addr_num;
		int addr_offset = blockNum % addr_num;
		uint16_t addrs[addr_num];
		int err = diskimg_readsector(fd, inp->i_addr[sector_offset], addrs);
		if(err < 0) return -1;	
		return addrs[addr_offset];
	} else {							// if it also uses the DOUBLE_INDIR_ADDR
		// the first layer
		int blockNum_in_double = blockNum - indir_addr_num;
		int sector_offset_1 = INDIR_ADDR;
		int addr_offset_1 = blockNum_in_double / addr_num;
		uint16_t addrs_1[addr_num];
		int err_1 = diskimg_readsector(fd, inp->i_addr[sector_offset_1], addrs_1);
		if(err_1 < 0) return -1;

		// the second layer
		int sector_2 = addrs_1[addr_offset_1];
		int addr_offset_2 = blockNum_in_double % addr_num;
		uint16_t addrs_2[addr_num];
		int err_2 = diskimg_readsector(fd, sector_2, addrs_2);
		if(err_2 < 0) return -1;
		return addrs_2[addr_offset_2];
	}	
}



///////////////

// int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
//     int inode_file_size = inode_getsize(inp);
//     if (inode_file_size == 0) return -1;

//     int inode_blocks = (inode_file_size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
//     if (blockNum < 0 || blockNum >= inode_blocks) return -1;

//     if (inode_file_size <= DISKIMG_SECTOR_SIZE * 8) {
//         return inp->i_addr[blockNum];
//     }

//     int16_t indir_block_num = blockNum / 256;
//     int16_t offset = blockNum % 256;
//     int16_t *indir_block = malloc(DISKIMG_SECTOR_SIZE);
//     if (indir_block == NULL) return -1; 
//     if (indir_block_num < 7) {
//         if (diskimg_readsector(fs->dfd, inp->i_addr[indir_block_num], indir_block) == -1) {
//             free(indir_block);
//             return -1;
//         } else {
//             int16_t block = indir_block[offset];
//             free(indir_block);
//             return block;
//         }
//     }

//     if (indir_block_num >= 7) {
//         int16_t second_indir_block_num = (blockNum - 7 * 256) / 256;
//         int16_t second_offset = (blockNum - 7 * 256) % 256;

//         if (diskimg_readsector(fs->dfd, inp->i_addr[7], indir_block) == -1) {
//             free(indir_block);
//             return -1;
//         } else {
//             int16_t *second_indir_block = malloc(DISKIMG_SECTOR_SIZE);
//             if (second_indir_block == NULL) {
//                 free(indir_block);
//                 return -1;
//             }

//             if (diskimg_readsector(fs->dfd, indir_block[second_indir_block_num], second_indir_block) == -1) {
//                 free(second_indir_block);
//                 free(indir_block);
//                 return -1;
//             }

//             int16_t block = second_indir_block[second_offset];
//             free(indir_block);
//             free(second_indir_block);
//             return block;
//         }
//     }

//     free(indir_block); 
//     return -1; 
// }

// int inode_getsize(struct inode *inp) {
//     return ((inp->i_size0 << 16) | inp->i_size1);
// }
// ///////////////




int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}