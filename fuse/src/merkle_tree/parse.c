#include "parse.h"

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

int compute_merkle(FILE **fp, merkle_tree *mt, char **result) {
    //File does not exist
    if (*fp == NULL) {
        log_msg("%s\n", "ERROR : File does not exist");
        return -1;
    }

    //Compute file size, and goes back to the beginning of the file
    fseek(*fp, 0L, SEEK_END);
    int file_size = ftell(*fp);
    fseek(*fp, 0L, SEEK_SET);

    //Reads file into file_string
    char *file_string = malloc(file_size + 1);
    fread(file_string, file_size, 1, *fp);

    //Compute array
    int nb_of_pages;
    int tree_size;


    nb_of_pages = (int) file_size / PAGE_LENGTH;
    if (file_size % PAGE_LENGTH == 0) {
        tree_size = compute_tree_size(nb_of_pages);
    }
    else {
        tree_size = compute_tree_size(nb_of_pages + 1);
    }
    char **parsed_file = (char **) calloc(tree_size, sizeof(char **) * sizeof(char *) * PAGE_LENGTH );
    *result = (char *) malloc(sizeof(char *) * HASH_SIZE * 2 * tree_size);

    for (int i = 0; i < tree_size ; i++){
      *parsed_file = calloc(PAGE_LENGTH, sizeof(char *));
      parsed_file += PAGE_LENGTH;
    }
    parsed_file = parsed_file - tree_size*PAGE_LENGTH;

    log_msg("%s\n", "Parsing first part");
    for (int i = 0 ; i < nb_of_pages ; i ++) {
        strncpy(*parsed_file, file_string, PAGE_LENGTH);
        log_msg("%d\n",strlen(*parsed_file));
        parsed_file += PAGE_LENGTH;
        file_string += PAGE_LENGTH;
    }

    log_msg("%s\n", "Parsing last part");

    if (file_size % PAGE_LENGTH == 0) {
      for (int i = nb_of_pages; i < tree_size ; i++){
        log_msg("%s\n", "nan mais allo");
        *parsed_file = malloc(sizeof(char*) * strlen(" "));
        *parsed_file = " ";
        parsed_file++;
      }
    }
    else {
        strncpy(*parsed_file, file_string, strlen(file_string) );
        parsed_file += strlen(file_string);

        for (int i = nb_of_pages + 1; i < tree_size ; i++){
          *parsed_file = malloc(sizeof(char*) * strlen(" "));
          *parsed_file = " ";
          parsed_file++;
        }
    }


    log_msg("%s\n", "Pointers back");

    //Pointers back at initial value
    if (file_size % PAGE_LENGTH == 0) {
      if (tree_size > nb_of_pages)
          parsed_file -= (tree_size - nb_of_pages);

      parsed_file -= ( (nb_of_pages) * PAGE_LENGTH ) ;
    }

    else {
        if (tree_size > nb_of_pages)
            parsed_file -= (tree_size - nb_of_pages - 1);
        parsed_file -= strlen(file_string);
        parsed_file -= ( (nb_of_pages) * PAGE_LENGTH ) ;
    }


    int height = (int) log2(tree_size) + 1;

    //Assigning values for merkle_tree calculation
    mt->tree_height = height;
    mt->nb_nodes    = 0;
    mt->nb_of_leaves = tree_size;
    log_msg("%s\n", "Building tree ...");
    if (build_tree(mt, parsed_file) == -1) {
        log_msg("%s\n", "ERROR : Tree could not build");
        return -1;
    }

    //Stores result in a string
    tree_to_string(mt, *result);
    log_msg("%s\n", *result);

    //Freeing memory
    log_msg("%s\n", "Free tree ...");
    for (int i = 1 ; i < mt->nb_nodes ; i ++) {
        //log_msg("%d\n", i);

        if (i > mt->nb_nodes / 2)
            free(mt->nodes[i].data);
        if (mt->nodes[i].hash)
            free(mt->nodes[i].hash);
    }
    log_msg("%s\n", "Free nodes ...");
    free(mt->nodes);

    log_msg("%s\n", "Free pages ...");
    for (int i = 0 ; i < nb_of_pages ; i ++) {
        free(*parsed_file);
        parsed_file += PAGE_LENGTH;
    }

    if (file_size % PAGE_LENGTH != 0) {
        free(*parsed_file);
    }

    parsed_file -= ( (nb_of_pages) * PAGE_LENGTH ) ;
    free(parsed_file);

    log_msg("%s\n", "Free filestring ...");
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

    //If there is less pages than threads, we call the single-threaded method
    if (nb_of_pages < nb_threads) {
        return compute_merkle(fp, mt, result);
    }

    int tree_size = compute_tree_size(nb_of_pages + 1);

    char **parsed_file = calloc(tree_size, sizeof(char **) * sizeof(char *) * PAGE_LENGTH );
    *result = malloc(sizeof(char *) * HASH_SIZE * 2 * tree_size);

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

    if (m_build_tree(mt, parsed_file, nb_threads) == -1)
        log_msg("%s\n", "ERROR : Tree could not build.");

    //Stores result in a string
    tree_to_string(mt, *result);
    log_msg("%s\n", *result);

    //Freeing memory
    for (int i = 1 ; i < mt->nb_nodes ; i ++) {

        if (i > mt->nb_nodes / 2)
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

int pages_in_need(int size, int offset, merkle_tree *mt, FILE **fp, char **result) {

    //File does not exist
    if (*fp == NULL) {
        log_msg("%s\n", "ERROR : File does not exist");
        return -1;
    }

    int first_page = (int) offset / PAGE_LENGTH;
    int last_page  = (int) (offset + size) / PAGE_LENGTH;
    int number = last_page - first_page + 1;

    int leaf_start_index = (1 << (mt->tree_height - 1));

    int indexes[number];
    for (int i = 0 ; i < number ; i ++) {
          indexes[i] = leaf_start_index + first_page + i;
    }
    fseek(*fp, 0L, SEEK_END);
    int file_size = ftell(*fp);
    fseek(*fp, 0L, SEEK_SET);
    int nb_of_pages = (int) file_size / PAGE_LENGTH;
    //If the new file has more pages than the tree can contain, compute a new one
    if (nb_of_pages > (mt->nb_nodes / 2))
        return compute_merkle(fp, mt, result);

    //Reads file into file_string
    char *file_string = malloc(file_size + 1);
    fread(file_string, file_size, 1, *fp);

    //Gets new data we need to change in the tree
    char **new_datas = calloc(number, sizeof(char **) * sizeof(char *) * PAGE_LENGTH);
    for (int i = 0; i < number ; i++){
      *new_datas = calloc(PAGE_LENGTH, sizeof(char *));
      new_datas += PAGE_LENGTH;
    }
    new_datas = new_datas - number*PAGE_LENGTH;
    file_string += (first_page * PAGE_LENGTH);

    for (int i = 0 ; i < number - 1 ; i ++) {
        strncpy(*new_datas, file_string, PAGE_LENGTH);
        new_datas += PAGE_LENGTH;
        file_string += PAGE_LENGTH;
    }

    //We consider the case the last page is concernerned
    if (last_page == nb_of_pages) {
      strlcpy(*new_datas, file_string, PAGE_LENGTH);

    }

    else {
        strlcpy(*new_datas, file_string, PAGE_LENGTH);
    }

    //Pointers back at initial value
    new_datas -= ((number-1) * PAGE_LENGTH);
    file_string -= ((first_page + number - 1) * PAGE_LENGTH);

    if (change_and_rebuild(mt, indexes, new_datas, number) == -1)
        return -1;

    //Freeing memory
    free(new_datas);
    free(file_string);

    *result = malloc(HASH_SIZE * mt->nb_nodes);
    tree_to_string(mt, *result);

    log_msg("%s\n", *result);
    return 0;

}
