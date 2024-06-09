
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
        char* next = strchr(path, '/');
        char* name;
        if (next == NULL){
            name = path;
        } else {
            name = strndup(path, next - path);
        }
        struct direntv6 dirEnt;
        if (directory_findname(fs, name, dirnum, &dirEnt) < 0){
            return -1;
        }
        dirnum = dirEnt.d_inumber;
        if (next == NULL){
            break;
        }
        path = next + strlen("/");
    }
    return dirnum;
}