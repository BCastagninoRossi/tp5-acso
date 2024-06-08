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
int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
  //Implement your code here
  struct inode in;
  if (inode_iget(fs, dirinumber, &in) < 0) {
    return -1;
  }
  int size = inode_getsize(&in);
  if (size == 0) {
    return -1;
  }
  int num_blocks = size / DISKIMG_SECTOR_SIZE;
  int aux = num_blocks % DISKIMG_SECTOR_SIZE;
  if (aux != 0) {
    num_blocks++;
  }
  for (int i = 0; i < num_blocks; i++) {
    if (file_getblock(fs, dirinumber, i, dirEnt) < 0) {
      return -1;
    }
    for (int j = 0; j < DISKIMG_SECTOR_SIZE / sizeof(struct direntv6); j++) {
      if (strcmp(dirEnt[j].d_name, name) == 0) {
        return 0;
      }
    }
  }
  return -1;
}
