#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merkletree.h"

#define MAXLINES 400000
#define PAGE_LENGTH 100
#define MAX_TREE_SIZE 64000000

//Computes closest power of two for the size of the file.
int compute_tree_size(int index);

//Parses a file to a char**, and computes the merkle_tree
int compute_merkle(FILE **fp, merkle_tree *mt, char **result);

int m_compute_merkle(FILE **fp, merkle_tree *mt, char **result, int nb_threads);

int pages_in_need(int size, int offset, merkle_tree *mt, FILE **fp, char **result);
