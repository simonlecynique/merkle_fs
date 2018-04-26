#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merkletree.h"

#define MAXLINES 400000
#define PAGE_LENGTH 10000

//Computes closest power of two for the size of the file.
int compute_tree_size(int index);

//Parses a file to a char**, and computes the merkle_tree
int compute_merkle(FILE **fp, merkle_tree *mt, char **result);
