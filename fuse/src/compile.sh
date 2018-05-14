gcc -DHAVE_CONFIG_H -I.    -D_FILE_OFFSET_BITS=64 -D_DARWIN_USE_64_BIT_INODE -I/usr/local/include/osxfuse/fuse -g -O2 -MT bbfs.o -MD -MP -MF .deps/bbfs.Tpo -lpthread -c -o bbfs.o bbfs.c
gcc -D_FILE_OFFSET_BITS=64 -D_DARWIN_USE_64_BIT_INODE -I/usr/local/include/osxfuse/fuse -lpthread merkle_tree/merkletree.c merkle_tree/sha3.c merkle_tree/parse.c -g -O2 -o bbfs bbfs.o log.o -L/usr/local/lib -losxfuse
make
