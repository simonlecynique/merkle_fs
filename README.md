# MerkleTree based Filesystem

A filesystem that guarantees file integrity with Merkle Trees.

## Principle

Each file has its own corresponding Merkle tree. The file is composed of pages (of PAGE_LENGTH characters).
Every modification of a file changes its Merkle tree. We therefore write a filesystem with FUSE that modifies the write function.

The Merkle tree is stored in the file's metadata, in an extended attribute.

At each write call, the system gets the previous Merkle Tree of the file that is stored its metadata, performs the write, and then computes the hashes of pages that were modified. The close call triggers computation of non-leaf nodes.

## Merkle Tree implementation

This part describes what can be found in the merkletree.c file.


A Merkle tree is composed of :

    -> (int) nb_nodes : Number of nodes in the tree.
    -> (int) tree_height : Height of the tree.
    -> (size_t) nb_of_leaves : Number of leaves of the tree.
    -> (node) *nodes : pointer to all the nodes of the tree.

A node is composed of :

    -> (char *) data : The string the node represents, and has to hash.
    -> (char *) hash : The hashed string (SHA256) of data if the node is a leaf, or a hash of the concatenation of the two children if not.


There are two implemented APIs :

* Single-Threaded

The main thread hashes node after node (leaves first, before going up the tree).

* Multi-Threaded

The number of threads has to be a power of two.
Each thread is assigned a part of the tree, and computes the hashes of this part.

For further details, please report to the merkletree.c file in the /fuse/src/merkle_tree directory.

## Parsing  

This part describes what can be found in the parse.c file.

The main purpose of the functions in this file is to parse a file into PAGE_LENGTH characters pages.

For further details, please report to the parse.c file in the /fuse/src/merkle_tree directory.

## The Filesystem

The filesystem definition can be found in the /fuse/src directory, in the bbfs.c file.

## How to run it

First, go to /fuse/src directory. To compile the filesystem, you need to run the ./compile command.
Now that the filesystem has compiled, you can mount any directory you want by running :
./bbfs rootdirectory mountdirectory

A Makefile in the fuse/example directory creates a default directory and mounts it into another.

To see a file's extended attribute, run the command xattr -l filename
