
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    //Implement code here
    if (strcmp(pathname, "/") == 0) {
        return ROOT_INUMBER;
    }
    const char* path = pathname + strlen("/");
    int dirnum = ROOT_INUMBER;
    while (*path != '\0'){
        const char* next = strchr(path, '/');
        if (next == NULL){
            struct direntv6 dirEnt;
            if (directory_findname(fs, path, dirnum, &dirEnt) < 0){
            return -1;
            }
            return dirEnt.d_inumber;
        } else {
            char* name;
            name = strndup(path, next - path);
            struct direntv6 dirEnt;
            if (directory_findname(fs, name, dirnum, &dirEnt) < 0){
                return -1;
            }
            dirnum = dirEnt.d_inumber;
            path = next + strlen("/");
        }
    }
    return -1;
}