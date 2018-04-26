#include "sha3.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>

#define HASH_SIZE 256
#define BYTE_SIZE 8
#define HEX 4

//Node of Merkle Tree definition
typedef struct {
    char *data;
    char *hash;
} node;

//Merkle Tree definition
typedef struct {
    int nb_nodes;
    int tree_height;
    size_t data_blocks;
    node *nodes;
} merkle_tree;

//MultiThread argument definition
typedef struct {
    merkle_tree *mt;
    int start_index;
    int nb_leaves;
} threaded_arg;

//Builds Merkle tree based on a data table to fill the leaves.
int build_tree(merkle_tree *mt, char **data_table);

//Computes hash of node i.
int hash_node(merkle_tree *mt, int i);

void set_tree_datas(merkle_tree *mt, char **data_table);

int m_build_tree(merkle_tree *mt, char **data_table, int nb_of_threads);

void *m_hash_nodes(void *arg);

void print_tree(merkle_tree *mt);

//Changes tree data of indexes in the argument array. The data to be inserted
//are in the datas argument. The number of data to change is number.
int change_and_rebuild(merkle_tree *mt, int indexes[], char **datas, int number);

//Compares two trees : when different node is found,
//searches until finding all different leaves.
void compare_trees(merkle_tree *mt_a, merkle_tree *mt_b, int index);

//Transforms tree to a string.
void tree_to_string(merkle_tree *mt, char tree[]);

//Transforms string to tree.
void string_to_tree(merkle_tree *mt, char *tree_string);

void free_merkle_tree(merkle_tree *mt);
