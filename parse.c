#include "parse.h"

void parse_file(FILE **fp, merkle_tree *mt, char **result) {

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char **lines = malloc(MAXLINES * 1000);
    //char *lines[MAXLINES];
    int index = 0;
    int total_size = 0;

    //File does not exist
    if (*fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(lines, &len, *fp)) != -1) {
                //printf("%s\n", *lines);
                lines = lines + read;
                // lines[index] = (char *) malloc(read);
                // lines[index][0] = '\0';
                // //lines[index] = line;
                // strcat(lines[index], line);
                // //strcpy(lines[index],line);
                index++;
                total_size += read;
                // printf("%s\n", lines[index]);
    }

    if (line)
        free(line);

    int tree_size = compute_tree_size(index);
    for (int i = index; i < tree_size ; i++){
      *lines = " ";
      lines++;
      total_size++;
    }

    lines -= total_size;
    int height = (int) log2(tree_size) + 1;
    mt->tree_height = height;
    mt->nb_nodes = 0;
    mt->data_blocks = tree_size;

    build_tree(mt, lines);
    free(lines);
    *result = (char *) malloc(HASH_SIZE * mt->nb_nodes);
    tree_to_string(mt, *result);


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
