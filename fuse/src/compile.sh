gcc -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse -g -O2 -MT bbfs.o -MD -MP -MF .deps/bbfs.Tpo -pthread -c -o bbfs.o bbfs.c
gcc -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse -pthread merkle_tree/merkletree.c merkle_tree/sha3.c merkle_tree/parse.c -g -O2 -o bbfs bbfs.o log.o -lfuse -lbsd -lm
