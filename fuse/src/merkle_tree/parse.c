#include "parse.h"

int compute_tree_size(int index) {
    unsigned int v = index;

    if (v == 0)
        return 1;

    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

int compute_merkle(FILE **fp, merkle_tree *mt, char **result) {
    //File does not exist
    if (*fp == NULL)
        return -1;

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
    char **parsed_file = (char **) calloc(tree_size, sizeof(char **) * sizeof(char *) * PAGE_LENGTH );
    *result = (char *) malloc(sizeof(char *) * HASH_SIZE * 2 * tree_size);

    for (int i = 0; i < tree_size ; i++){
      *parsed_file = calloc(PAGE_LENGTH, sizeof(char *));
      parsed_file += PAGE_LENGTH;
    }
    parsed_file = parsed_file - tree_size*PAGE_LENGTH;

    for (int i = 0 ; i < nb_of_pages ; i ++) {
        strncpy(*parsed_file, file_string, PAGE_LENGTH);
        parsed_file += PAGE_LENGTH;
        file_string += PAGE_LENGTH;
    }

    //*parsed_file = (char *)calloc(strlen(file_string), sizeof(char *));
    strncpy(*parsed_file, file_string, strlen(file_string) );
    parsed_file += strlen(file_string);

    for (int i = nb_of_pages + 1; i < tree_size ; i++){
      *parsed_file = malloc(sizeof(char*) * strlen(" "));
      *parsed_file = " ";
      parsed_file++;
    }

    //Pointers back at initial value
    if (tree_size > nb_of_pages)
        parsed_file -= (tree_size - nb_of_pages - 1);
    parsed_file -= strlen(file_string);
    parsed_file -= ( (nb_of_pages) * PAGE_LENGTH ) ;

    int height = (int) log2(tree_size) + 1;

    //Assigning values for merkle_tree calculation
    mt->tree_height = height;
    mt->nb_nodes    = 0;
    mt->nb_of_leaves = tree_size;

    if (build_tree(mt, parsed_file) == -1)
        return -1;

    //Stores result in a string
    tree_to_string(mt, *result);

    for (int i = 1 ; i < mt->nb_nodes ; i ++) {
        if (mt->nodes[i].data)
            free(mt->nodes[i].data);
        if (mt->nodes[i].hash)
            free(mt->nodes[i].hash);
    }
    free(mt->nodes);

    for (int i = 0 ; i < nb_of_pages ; i ++) {
        free(*parsed_file);
        parsed_file += PAGE_LENGTH;
    }

    free(*parsed_file);
    parsed_file -= ( (nb_of_pages) * PAGE_LENGTH ) ;
    free(parsed_file);

    file_string -= (nb_of_pages) * PAGE_LENGTH;
    free(file_string);

    return 0;
}

int m_compute_merkle(FILE **fp, merkle_tree *mt, char **result, int nb_threads) {
    //File does not exist
    if (*fp == NULL)
        return -1;

    //Compute file size, and goes back to the beginning of the file
    fseek(*fp, 0L, SEEK_END);
    int file_size = ftell(*fp);
    fseek(*fp, 0L, SEEK_SET);

    //Reads file into file_string
    char *file_string = malloc(file_size + 1);
    fread(file_string, file_size, 1, *fp);

    //Compute array
    int nb_of_pages = (int) file_size / PAGE_LENGTH;

    if (nb_of_pages < nb_threads) {
        return compute_merkle(fp, mt, result);
    }
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
    if (tree_size > nb_of_pages)
        parsed_file -= (tree_size - nb_of_pages - 1);
    parsed_file -= strlen(file_string);
    parsed_file -= ( (nb_of_pages) * PAGE_LENGTH ) ;
    file_string -= (nb_of_pages) * PAGE_LENGTH;

    int height   = (int) log2(tree_size) + 1;

    //Assigning values for merkle_tree calculation
    mt->tree_height = height;
    mt->nb_nodes    = 0;
    mt->nb_of_leaves = tree_size;

    if (m_build_tree(mt, parsed_file, nb_threads) == -1)
        //log_msg("%s\n", "oh putain merde");

    free(file_string);
    free(parsed_file);

    //Stores result in a string
    *result = (char *) malloc(HASH_SIZE * 2 * mt->nb_nodes);

    tree_to_string(mt, *result);

    return 0;
}

int pages_in_need(int size, int offset, merkle_tree *mt, FILE **fp, char **result) {

    //File does not exist
    if (*fp == NULL)
        return -1;

    int first_page = (int) offset / PAGE_LENGTH;
    int last_page  = (int) (offset + size) / PAGE_LENGTH;
    int number = last_page - first_page + 1;

    int indexes[number];
    for (int i = 0 ; i < number ; i ++) {
          indexes[i] = first_page + i;
    }

    fseek(*fp, 0L, SEEK_END);
    int file_size = ftell(*fp);
    fseek(*fp, 0L, SEEK_SET);

    int nb_of_pages = (int) file_size / PAGE_LENGTH;

    //If the new file has more pages than the tree can contain, compute a new one
    if (nb_of_pages > mt->nb_of_leaves)
        return compute_merkle(fp, mt, result);


    //Reads file into file_string
    char *file_string = malloc(file_size + 1);
    fread(file_string, file_size, 1, *fp);

    // log_msg("%s\n", "in pages_in_need");
    // log_msg("%d\n", nb_of_pages);
    // log_msg("%d\n", first_page);
    // log_msg("%d\n", last_page);


    //Gets new data we need to change in the tree
    char **new_datas = (char **) malloc(sizeof(char *) * PAGE_LENGTH * number );

    file_string += (first_page * PAGE_LENGTH);

    //TODO - If the first page and the last page is the same ?

    //TODO - If if last_page is the end of the file ?

    for (int i = 0 ; i < number - 1 ; i ++) {
        *new_datas = (char *)malloc(PAGE_LENGTH);
        strncpy(*new_datas, file_string, PAGE_LENGTH);
        new_datas += PAGE_LENGTH;
        file_string += PAGE_LENGTH;
    }

    if (last_page == nb_of_pages) {
      *new_datas = (char *)malloc(strlen(file_string));
      strncpy(*new_datas, file_string, strlen(file_string));

    }

    else {
        *new_datas = (char *)malloc(PAGE_LENGTH);
        strncpy(*new_datas, file_string, PAGE_LENGTH);
    }


    //Pointers back at initial value
    new_datas -= ((number-1) * PAGE_LENGTH);
    file_string -= ((first_page + number - 1) * PAGE_LENGTH);

    if (change_and_rebuild(mt, indexes, new_datas, number) == -1)
        return -1;

    free(new_datas);
    free(file_string);

    *result = (char *) malloc(HASH_SIZE * mt->nb_nodes);
    tree_to_string(mt, *result);

    return 0;

}
