#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merkletree.h"

#define MAXLINES 100000

typedef struct {
    char* *lines;
    int size;
    int tree_size;
} parsed_file;

//Computes closest power of two for the size of the file.
int compute_tree_size(int index);

//Parses a file to a char**, and computes the merkle_tree
void parse_file(const char * filename, merkle_tree *mt);
