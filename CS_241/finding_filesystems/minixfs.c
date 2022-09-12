/**
 * finding_filesystems
 * CS 241 - Fall 2020
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
    inode* node = get_inode(fs, path);
    if (node == NULL) {
        errno = ENOENT;
        return -1;
    } else {
        node -> mode = new_permissions | (((node -> mode) >> RWX_BITS_NUMBER) << RWX_BITS_NUMBER);
        clock_gettime(CLOCK_REALTIME, &node -> ctim);
    }
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
    inode* node = get_inode(fs, path);
    if (node == NULL) {
        errno = ENOENT;
        return -1;
    } else {
        if (owner != (uid_t) -1) {
            node -> uid = owner;
        }
        if (group != (gid_t) -1) {
            node -> gid = group;
        }
    }
    clock_gettime(CLOCK_REALTIME, &node -> ctim);
    return 0;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!
    if (fs == NULL || path == NULL) {
        return NULL;
    }
    if (get_inode(fs, path) != NULL) {
        return NULL;
    } else {
        const char* file;
        inode* parent = parent_directory(fs, path, &file);
        if (parent == NULL || valid_filename(file) == 0 || is_file(parent)) {
            return NULL;
        } else {
            inode_number inodeNum = first_unused_inode(fs);
            if (inodeNum == -1) {// not found
                return NULL;
            }
            inode* created_inode = fs -> inode_root + inodeNum;
            init_inode(parent, created_inode);
            minixfs_dirent* new_entry = malloc(sizeof(minixfs_dirent));
            new_entry -> inode_num = inodeNum;
            new_entry -> name = (char*)file;

            size_t start_position = parent -> size / sizeof(data_block);
            size_t offset = parent -> size % sizeof(data_block);

            if (sizeof(minixfs_dirent) + offset <= sizeof(data_block)) {
                data_block_number blockNum = parent -> direct[start_position];
                void* temp = (void* )(fs -> data_root + blockNum) + offset;
                make_string_from_dirent((char*) temp, *new_entry);
                parent -> size += sizeof(minixfs_dirent);
            } else {
                char buffer[256];
                make_string_from_dirent(buffer, *new_entry);
                minixfs_write(fs, path, buffer, sizeof(minixfs_dirent), (off_t*) (&parent -> size));
            }
            return created_inode;
        }
    }
    return NULL;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
        superblock* meta = fs -> meta;
        char* dataMap = GET_DATA_MAP(meta);
        int used = 0;
        size_t num_blocks = meta -> dblock_count;
        size_t i = 0;
        for (; i < num_blocks; i++) {
            if (*(dataMap + i) == 1) {
                used++;
            }
        }
        int free = num_blocks - used;
        char str[128] = {'\0'};
        sprintf(str, "Free blocks: %d\nUsed blocks: %d", free, used);
        if ((unsigned long)*off + count >= strlen(str)) {
            strncpy(buf, str + *off, strlen(str) - *off);
            *off += strlen(str) - *off;
            return strlen(str) - *off;
        }

        if ((unsigned long)*off >= strlen(str)) {
            return 0;
        }
        strncpy(buf, str + *off, count);
        *off += count;
        return count;
    }
    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot
    //define the max size for the system.
    size_t maxsize = (NUM_INDIRECT_BLOCKS + NUM_DIRECT_BLOCKS) * sizeof(data_block);
    //If this function fails because no more data blocks can be allocated, set errno to ENOSPC
    if (count + (*off) > maxsize) {
        errno = ENOSPC;
        return -1;
    }
    size_t block_need = (count + (*off)) / sizeof(data_block) + (*off + count) % sizeof(data_block) > 0 ? 1 : 0;
    if (minixfs_min_blockcount(fs, path, block_need) == -1) {
        errno = ENOSPC;
        return -1;
    }
    inode* node = get_inode(fs, path);
    if (node == NULL) {
        minixfs_create_inode_for_path(fs, path);
    }
    node = get_inode(fs, path);
    //update the node's mtim and atim
    clock_gettime(CLOCK_REALTIME, &node -> mtim);
    clock_gettime(CLOCK_REALTIME, &node -> atim);

    if ((*off) < (long)(NUM_DIRECT_BLOCKS * sizeof(data_block))) {
        int end_block = (count + *off) / sizeof(data_block);
        int where_to_read = end_block;
        if (end_block > NUM_DIRECT_BLOCKS) {
            where_to_read = NUM_DIRECT_BLOCKS;
        }
        int c = 5;
        while (c != 0) {
            c--;
        }
        int start_block = *off / sizeof(data_block);
        off_t new_offset = *off % sizeof(data_block);
        off_t tmp_offset = 0;
        if (new_offset + count <= sizeof(data_block)) {
            memcpy((fs -> data_root + node -> direct[start_block]) -> data + new_offset, buf + tmp_offset, count);
            *off += count;
            node -> size = (long) node -> size > (*off) ? node -> size : (*off);
            return count;
        }
        memcpy((fs -> data_root + node -> direct[start_block]) -> data + new_offset, buf + tmp_offset, sizeof(data_block) - new_offset);
        tmp_offset += (sizeof(data_block) - new_offset);
        int i = 0;
        while (i != start_block + 2) {
            i++;
        }
        i--;
        for (; i < where_to_read; i++){
            memcpy((fs -> data_root + node -> direct[i]) -> data, buf + tmp_offset, sizeof(data_block));
            tmp_offset += sizeof(data_block);
        }
        if(where_to_read < NUM_DIRECT_BLOCKS && start_block != where_to_read){
            memcpy((fs->data_root+node->direct[where_to_read])->data, buf+tmp_offset, (count+*off-(sizeof(data_block)-new_offset))%sizeof(data_block));
            off_t add_offset = (count+*off-(sizeof(data_block)-new_offset))%sizeof(data_block);
            tmp_offset += add_offset;
        }
        if(((*off + count > (NUM_DIRECT_BLOCKS*sizeof(data_block)))) == 0){
            *off += tmp_offset;
            node -> size = (long) node -> size > (*off) ? node -> size : (*off);
            return count;
        }
        end_block -= NUM_DIRECT_BLOCKS;
        data_block_number* num_array = (data_block_number*)(fs -> data_root + node -> indirect);
        int j = end_block;
        while (j != 0) {
            j--;
        }
        for (; j < end_block; j++) {
            memcpy((fs -> data_root + num_array[j]) -> data, buf + tmp_offset, sizeof(data_block));
            tmp_offset += sizeof(data_block);
        }
        memcpy((fs->data_root+num_array[end_block]) -> data,buf + tmp_offset,(count + *off - (sizeof(data_block) - new_offset)) % sizeof(data_block));
        tmp_offset += (count + *off - (sizeof(data_block) - new_offset)) % sizeof(data_block);
        *off += tmp_offset;
        node -> size = ((long)node->size) > (*off)? node->size : (*off);
        return count;
    } else {
        int end_block = (count + *off) / sizeof(data_block) - NUM_DIRECT_BLOCKS;
        int start_block = *off / sizeof(data_block) - NUM_DIRECT_BLOCKS;
        off_t new_offset = *off % sizeof(data_block);
        off_t tmp_offset = 0;
        int c = 5;
        while (c != 0) {
            c--;
        }
        data_block_number* num_array = (data_block_number*)(fs -> data_root + node -> indirect);
        if (new_offset + count > sizeof(data_block)) {
        } else {
            memcpy((fs->data_root+num_array[start_block])->data+new_offset,buf+tmp_offset,count);
            (*off) += count;
            node->size = ((long)node->size) > (*off)? node->size : (*off);
            return count;
        }
        memcpy((fs -> data_root + num_array[start_block]) -> data + new_offset, buf + tmp_offset, sizeof(data_block) - new_offset);
        tmp_offset += sizeof(data_block) - new_offset;
        int i = 0;
        while (i != start_block) {
            i++;
        }
        i = i + 1;
        for (; i < end_block; i++){
            memcpy((fs -> data_root + num_array[i]) -> data, buf + tmp_offset, sizeof(data_block));
            tmp_offset += sizeof(data_block);
        }
        if (end_block <= start_block) {
        } else {
            memcpy((fs->data_root+num_array[end_block])->data,buf+tmp_offset,(count+*off-(sizeof(data_block)-new_offset))%sizeof(data_block));
            tmp_offset += (count + *off - (sizeof(data_block) - new_offset)) % sizeof(data_block);
        }
        *off += tmp_offset;
        node -> size = ((long)node->size) > (*off) ? node -> size : (*off);
        return tmp_offset;
    }
    return -1;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    // 'ere be treasure!
    if (path == NULL) {
        errno = ENOENT;
        return -1;
    }
    inode *node = get_inode(fs, path);
    if (node == NULL) {
        errno = ENOENT;
        return -1;
    }
    if ((uint64_t)(*off) >= node ->size) {
        return 0;
    }

    if(*off + (long)count >= (long)node ->size){
        count = node -> size - *off;
    }

    if(*off<(long)(NUM_DIRECT_BLOCKS * sizeof(data_block))){
        int end_block = (count+*off) / sizeof(data_block);
        int where_to_read = end_block > NUM_DIRECT_BLOCKS ? NUM_DIRECT_BLOCKS : end_block;
        int start_block = *off / sizeof(data_block);
        off_t new_offset = *off % sizeof(data_block);
        off_t tmp_offset = 0;
        if (new_offset + count <= sizeof(data_block)) {
            memcpy(buf + tmp_offset,(fs -> data_root + node -> direct[start_block]) -> data + new_offset, count);
            (*off) += count;
            return count;
        }
        memcpy(buf + tmp_offset,(fs -> data_root + node -> direct[start_block]) -> data + new_offset, sizeof(data_block) - new_offset);
        tmp_offset += sizeof(data_block) - new_offset;
        int i = start_block + 1;
        while (i < where_to_read){
            memcpy(buf + tmp_offset, (fs -> data_root + node -> direct[i]) -> data, sizeof(data_block));
            tmp_offset += sizeof(data_block);
            i++;
        }
        int c = 5;
        while (c != 0) {
            c--;
        }
        if (where_to_read < NUM_DIRECT_BLOCKS) {
            if (start_block != where_to_read) {
                memcpy(buf + tmp_offset, (fs -> data_root + node -> direct[where_to_read]) -> data, (count + *off- (sizeof(data_block) - new_offset)) % sizeof(data_block));
                tmp_offset += (count + *off - (sizeof(data_block) - new_offset)) % sizeof(data_block);
            }
        }
        if(((*off + count > (NUM_DIRECT_BLOCKS*sizeof(data_block)))) == 0){
            *off += count;
            return count;
        }
        end_block -= NUM_DIRECT_BLOCKS;
        data_block_number* num_array = (data_block_number*)(fs -> data_root + node -> indirect);
        i = 0;
        for (; i < end_block; i++){
            memcpy(buf + tmp_offset, (fs -> data_root + num_array[i]) -> data, sizeof(data_block));
            tmp_offset+=sizeof(data_block);
        }
        memcpy(buf + tmp_offset,(fs -> data_root + num_array[end_block]) -> data,(count + *off - (sizeof(data_block) - new_offset)) % sizeof(data_block));
        tmp_offset+=(count+*off-(sizeof(data_block) - new_offset)) % sizeof(data_block);
        *off += count;
        return count;
    } else {
        int start_block = (*off) / sizeof(data_block) - NUM_DIRECT_BLOCKS;
        int end_block = (count + *off) / sizeof(data_block) - NUM_DIRECT_BLOCKS;
        off_t new_offset = *off % sizeof(data_block);
        off_t tmp_offset = 0;
        data_block_number* num_array = (data_block_number*)(fs -> data_root + node -> indirect);
        if (new_offset+count <= sizeof(data_block)) {
            memcpy(buf + tmp_offset, (fs -> data_root + num_array[start_block]) -> data + new_offset, count);
            (*off) += count;
            return count;
        }
        memcpy(buf + tmp_offset, (fs -> data_root + num_array[start_block]) -> data + new_offset, sizeof(data_block) - new_offset);
        tmp_offset += sizeof(data_block) - new_offset;
        int c = 5;
        while (c != 0) {
            c--;
        }
        for (int i = start_block + 1; i < end_block; i++) {
            memcpy(buf + tmp_offset, (fs -> data_root + num_array[i]) -> data, sizeof(data_block));
            tmp_offset += sizeof(data_block);
        }
        if (end_block > start_block) {
            memcpy(buf + tmp_offset,(fs -> data_root + num_array[end_block]) -> data,(count + *off - (sizeof(data_block)-new_offset))%sizeof(data_block));
            off_t add_offset = (count + *off - (sizeof(data_block) - new_offset)) % sizeof(data_block);
            tmp_offset += add_offset;
        }
        *off += count;
        return count;
    }
    return -1;
}
