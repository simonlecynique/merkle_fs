#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merkletree.h"

#define MAX_TREE_SIZE 64000000

//Computes closest power of two of the input.
int compute_tree_size(int index);

//Computes the Merkle tree given the parsed file.
int compute_merkle(FILE **fp, merkle_tree *mt, char **result);

//Computes the Merkle tree given the parsed file. Multithreaded API.
int m_compute_merkle(FILE **fp, merkle_tree *mt, char **result, int nb_threads);

//Updates the Merkle tree after a change has been made to the file.
int pages_in_need(int size, int offset, merkle_tree *mt, FILE **fp, char **result);

//If a write changes a full page, updates the Merkle tree given a buffer
//without opening the file.
int quick_change(int size, int offset, char **buf, merkle_tree *mt, char ** result);

//Computes the hashes of the nodes that are not leaves.
int root_calculation(merkle_tree *mt, char **result);
