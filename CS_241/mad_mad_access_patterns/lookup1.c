/**
 * mad_mad_access_patterns
 * CS 241 - Fall 2020
 * partner: chenlei2, minhaoj2, jingz15, hangel2
 */
#include "tree.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/

void search(char* word, FILE* f, uint32_t offset) {
  if(!offset) {
    printNotFound(word);
    return;
  }
  fseek(f, offset, SEEK_SET);
  BinaryTreeNode n;
  fread(&n, sizeof(BinaryTreeNode), 1, f);
  fseek(f, sizeof(n) + offset, SEEK_SET);
  char buf[20];
  fread(buf, 20, 1, f);
  int res = strcmp(buf, word);
  
  if(!res) {
    printFound(word, n.count, n.price);
    return;
  }
  else if(res < 0) {
    return search(word, f, n.right_child);
  }
  else 
    return search(word, f, n.left_child);
}

int main(int argc, char **argv) {
  if(argc < 3) {
    printArgumentUsage();
    exit(1);
  }

  FILE *f = fopen(argv[1], "r");
  if(!f) {
    openFail(argv[1]);
    exit(2);
  }

  char first[4];
  fread(first, 1, 4, f);
  if(strcmp(first, "BTRE")) {
    formatFail(argv[1]);
    exit(2);
  }

  for(int i = 2; i < argc; i++) {
    search(argv[i], f, 4);
  }
  fclose(f);
  return 0;
}
