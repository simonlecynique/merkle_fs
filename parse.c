#include "parse.h"

void parse_file(FILE **fp, merkle_tree *mt, char **result) {

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char **lines = malloc(MAXLINES * 1000);
    int index = 0;
    int total_size = 0;

    //File does not exist
    if (*fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(lines, &len, *fp)) != -1) {
                lines = lines + read;
                index++;
                total_size += read;
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

    //Assigning values for merkle_tree calculation
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

void compute_merkle(FILE **fp, merkle_tree *mt, char **result) {

    //File does not exist
    if (*fp == NULL)
        exit(EXIT_FAILURE);

    //Compute file size, and goes back to the beginning of the file
    fseek(*fp, 0L, SEEK_END);
    int file_size = ftell(*fp);
    fseek(*fp, 0L, SEEK_SET);

    //Reads file into file_string
    char *file_string = malloc(file_size + 1);
    fread(file_string, file_size, 1, *fp);

    //Compute array
    int nb_of_pages = (int) file_size / PAGE_LENGTH;
    int tree_size = compute_tree_size(nb_of_pages);
    char **parsed_file = (char **)malloc(sizeof(char *) * PAGE_LENGTH * tree_size );

    for (int i = 0 ; i < nb_of_pages ; i ++) {
        *parsed_file = (char *)malloc(PAGE_LENGTH);
        strncpy(*parsed_file, file_string, PAGE_LENGTH);
        parsed_file += PAGE_LENGTH;
        file_string += PAGE_LENGTH;
    }

    *parsed_file = (char *)malloc(strlen(file_string));
    strncpy(*parsed_file, file_string, strlen(file_string) );
    parsed_file += strlen(file_string);

    for (int i = nb_of_pages + 1; i < tree_size ; i++){
      *parsed_file = " ";
      parsed_file++;
    }

    //Pointers back at initial value
    parsed_file -= (tree_size - nb_of_pages - 1);
    parsed_file -= strlen(file_string);
    parsed_file -= ( (nb_of_pages) * PAGE_LENGTH ) ;
    file_string -= (nb_of_pages) * PAGE_LENGTH;

    int height = (int) log2(tree_size) + 1;

    //Assigning values for merkle_tree calculation
    mt->tree_height = height;
    mt->nb_nodes = 0;
    mt->data_blocks = tree_size;

    build_tree(mt, parsed_file);
    free(file_string);

    //Stores result in a string
    *result = (char *) malloc(HASH_SIZE * mt->nb_nodes);
    tree_to_string(mt, *result);

}
