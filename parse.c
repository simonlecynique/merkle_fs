#include "parse.h"

void parse_file(const char * filename, merkle_tree *mt) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    char *lines[MAXLINES];

    int index = 0;

    fp = fopen(filename, "r");

    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
                lines[index] = line;
                //printf("%s", lines[index] );
                index++;
    }

    fclose(fp);
    if (line)
        free(line);

    int tree_size = compute_tree_size(index);
    for (int i = index; i < tree_size ; i++){
      lines[i] = " ";
    }

    int height = (int) log2(tree_size) + 1;

    mt->tree_height = height;
    mt->nb_nodes = 0;
    mt->data_blocks = tree_size;

    build_tree(mt, lines);
    
}

int compute_tree_size(int index) {
    unsigned int v = index;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

// int main(int argc, char const *argv[]) {
//     char **lines;
//     merkle_tree mt;
//     parse_file("parse.c", &mt);
//     parse_file("parse.h", &mt);
//     parse_file("merklelog.c", &mt);
//     parse_file("merkletree.h", &mt);
//     //parse_file("long.txt", &mt);
//     parse_file("testfile.txt", &mt);
//     return 0;
// }
