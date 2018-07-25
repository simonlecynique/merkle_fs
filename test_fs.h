#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define PAGE_LENGTH 65536

int test_fs(int nb_threads, int nb_files_per_threads, int size_of_file, int size_to_write, char *path);

void *create_files(void *arg);

void *rand_writes(void *arg);

typedef struct {
    char *name;
    int nb_of_files;
    int size_of_file;
    int size_to_write;
} arguments;
