#include "sha3.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>

#define HASH_SIZE 256
#define BYTE_SIZE 8
#define HEX 4
#define PAGE_LENGTH 4096
#define BUFFER_MODE 1
#define FILE_MODE 0

// DEFINITIONS------------------------------------------------------------------

//Node of Merkle Tree definition
typedef struct {
    char *data;
    char *hash;
} node;

//Merkle Tree definition
typedef struct {
    int nb_nodes;
    int tree_height;
    size_t nb_of_leaves;
    node *nodes;
} merkle_tree;

//MultiThread argument definition
typedef struct {
    merkle_tree *mt;
    int start_index;
    int nb_leaves;
} threaded_arg;

//SINGLE THREADED---------------------------------------------------------------

//Computes the Merkle tree of the datas.
int build_tree(merkle_tree *mt, char **datas);

//Hashes the data of a node.
int hash_node(merkle_tree *mt, int i);

//MULTI THREADED----------------------------------------------------------------

//Fills the data of leaves with a parsed file.
int set_tree_datas(merkle_tree *mt, char **datas);

//Computes the Merkle tree of the datas. Multithreaded API.
int m_build_tree(merkle_tree *mt, char **datas, int nb_of_threads);

//Hashes the data of several nodes. Multithreaded API.
void *m_hash_nodes(void *arg);

//------------------------------------------------------------------------------

//Prints whole tree.
void print_tree(merkle_tree *mt);

//Changes pages of tree and recomputes hashes.
int change_and_rebuild(merkle_tree *mt, int indexes[], char **datas, int number, int mode);

//Prints differences between two Merkle trees.
void compare_trees(merkle_tree *mt_a, merkle_tree *mt_b, int index);

//Formats Merkle tree to string.
void tree_to_string(merkle_tree *mt, char tree[]);

//Fills tree with hashes in string.
void string_to_tree(merkle_tree *mt, char *tree_string);

//Frees Merkle tree.
void free_merkle(merkle_tree *mt);
