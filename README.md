# MerkleTree based Filesystem

A filesystem that guarantees file integrity with Merkle Trees.

## SHA3 API Overview

The SHA3 functions were implemented by Andrey Jivsov (crypto@brainhub.org), found here on public domain on github : https://github.com/brainhub/SHA3IUF
Here is a concrete example of the API, the hash of the string "abc".

    sha3_context c;
    uint8_t *hash;

Single-buffer hashing:

    sha3_Init256(&c);
    sha3_Update(&c, "abc", 3);
    hash = sha3_Finalize(&c);
    // 'hash' points to a buffer inside 'c'
    // with the value of SHA3-256

Alternatively, IUF hashing:

    sha3_Init256(&c);
    sha3_Update(&c, "a", 1);
    sha3_Update(&c, "bc", 2);
    hash = sha3_Finalize(&c);

## Merkle Tree implementation

### Classes

A Merkle tree is composed of :

    -> (int) nb_nodes : Number of nodes in the tree.
    -> (int) tree_height : Height of the tree.
    -> (size_t) nb_of_leaves : Number of leaves of the tree.
    -> (node) *nodes : pointer to all the nodes of the tree.

A node is composed of :

    -> (char *) data : The string the node represents, and has to hash.
    -> (char *) hash : The hashed string of data.

### Functions

There are two implemented APIs :

* Single-Threaded

**hash_node** :

The function is used to hash the data of the node n° i. Acts differently whether the node is a leaf (just hashes the data), or not (hashes the concatenation of children leaves).

**build_tree** :

This function computes the whole tree hashes based on the strings pointer
when given a merkle_tree with the good number of nodes and the tree height.

* Multi-Threaded

**m_build_tree** :

Same as build_tree, but multithreaded. You give the number of threads you want to use as an additional argument. It has to be a power of two for now.

**m_hash_nodes** :

This multithreaded function computes the hashes of a portion of the tree.

**set_tree_datas** :

Just sets the data, used right before computing all the hashes.

* Data changes and comparisons

**change_and_rebuild** :

Changes the data pointed by the arguments and recomputes the nodes that are modified.

**compare_trees** :
This function compares two Merkle Trees on node n° i.
If the node is different, then compares the two children.

* Strings

**tree_to_string and string_to_tree** :
Turn the tree into string and vice-versa.

* Free Structure

**free_merkle_tree** :

Frees the structure.

## Parsing  
