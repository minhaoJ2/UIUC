/**
 * mad_mad_access_patterns
 * CS 241 - Fall 2020
 * partner: chenlei2, minhaoj2, jingz15, hangel2
 */
#include "tree.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/

void search(char* word, char* addr, uint32_t offset) {
  if(!offset) {
    printNotFound(word);
    return;
  }
  BinaryTreeNode *root = (BinaryTreeNode*) (addr + offset);
  int res1 = strcmp(word, root->word);
  int res2 = strcmp(root->word, word);
  if(!res1) {
    printFound(word, root->count, root->price);
    return;
  }
  else if(res2 < 0) {
    return search(word, addr, root->right_child);
  }
  else 
    return search(word, addr, root->left_child);
}

int main(int argc, char **argv) {
  if(argc < 3) {
    printArgumentUsage();
    exit(1);
  }

  int fd = open(argv[1], O_RDONLY);
  if(fd < 0) {
    openFail(argv[1]);
    exit(2);
  }

  struct stat info;
  int res = fstat(fd, &info);
  if(res != 0) {
    openFail(argv[1]);
    exit(2);
  }

  char* addr = mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if(addr == (void *) -1) {
    mmapFail(argv[1]);
    exit(2);
  }

  if(strncmp(addr, "BTRE", 4)) {
    formatFail(argv[1]);
    exit(2);
  }

  for(int i = 2; i < argc; i++) {
    search(argv[i], addr, 4);
  }

  close(fd);
  return 0;
}

