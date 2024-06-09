#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
// int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
//   //Implement your code here
//   struct inode in;
//   if (inode_iget(fs, dirinumber, &in) < 0) {
//     return -1;
//   }

// 	int is_dir = ((in.i_mode & IFMT) == IFDIR);
// 	if(!is_dir) return -1;

//   int size = inode_getsize(&in);
//   if (size <= 0) {
//     return -1;
//   }

//   int num_blocks = size / DISKIMG_SECTOR_SIZE;
//   int aux = num_blocks % DISKIMG_SECTOR_SIZE;
//   if (aux != 0) {
//     num_blocks++;
//   }

//   for (int i = 0; i < num_blocks; i++) {
//     struct direntv6 dirent_array[DISKIMG_SECTOR_SIZE / sizeof(struct direntv6)];
//     int valid_bytes = file_getblock(fs, dirinumber, i, dirent_array);
//     if(valid_bytes < 0) return -1;
//     int num_entries_in_block = valid_bytes / sizeof(struct direntv6);
//     for (int j = 0; j < num_entries_in_block; j++) {
//       if (strcmp(dirent_array[j].d_name, name) == 0) {
//         *dirEnt = dirent_array[j];
//         return 0;
//       }
//     }
//   }
//   return -1;
// }

int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
  //Implement your code here
  if (dirEnt == NULL) return -1;
  struct inode in;
  int err = inode_iget(fs, dirinumber, &in);
  if(err < 0) return -1;
  if ((in.i_mode & IFMT) != IFDIR) return -1;
  int size = inode_getsize(&in);
  if(size <= 0) return -1;
  int num_blocks = (size-1)/ DISKIMG_SECTOR_SIZE + 1;
  for (int i = 0; i < num_blocks; i++) {
    struct direntv6 dirent_array[DISKIMG_SECTOR_SIZE / sizeof(struct direntv6)];
    int valid_bytes = file_getblock(fs, dirinumber, i, dirent_array);
    if(valid_bytes < 0) return -1;
    int num_entries_in_block = valid_bytes / sizeof(struct direntv6);
    for (int j = 0; j < num_entries_in_block; j++) {
      if (strcmp(dirent_array[j].d_name, name) == 0) {
        *dirEnt = dirent_array[j];
        return 0;
      }
    }
  }
  return -1;
}
