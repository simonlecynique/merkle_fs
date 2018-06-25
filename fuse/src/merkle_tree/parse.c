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

int is_full(int file_size) {

    if (file_size % PAGE_LENGTH == 0)
        return 1;

    return 0;
}

int get_file_size(FILE **fp) {
  fseek(*fp, 0L, SEEK_END);
  int size = ftell(*fp);
  fseek(*fp, 0L, SEEK_SET);

  return size;
}

void free_file(char **parsed_file, int nb_of_pages, int is_full) {

  for (int i = 0 ; i < nb_of_pages ; i ++) {
      free(*parsed_file);
      parsed_file += PAGE_LENGTH;
  }

  if (!is_full) {
      free(*parsed_file);
  }

  parsed_file -= ((nb_of_pages) * PAGE_LENGTH);

  return;

}

void parse_file(char **parsed_file, char **file_string, int nb_of_pages, int tree_size, int page_full) {
    for (int i = 0 ; i < nb_of_pages ; i ++) {
        strncpy(*parsed_file, *file_string, PAGE_LENGTH);
        parsed_file += PAGE_LENGTH;
        *file_string += PAGE_LENGTH;
    }

    if (!page_full) {
      strncpy(*parsed_file, *file_string, strlen(*file_string) );
      parsed_file += strlen(*file_string);
    }

    for (int i = nb_of_pages + 1 - page_full; i < tree_size ; i++){
      *parsed_file = malloc(sizeof(char*) * strlen(" "));
      *parsed_file = " ";
      parsed_file++;
    }

    if (tree_size > nb_of_pages)
        parsed_file -= (tree_size - nb_of_pages - 1 + page_full);

    if (!page_full)
        parsed_file -= strlen(*file_string);

    parsed_file -= ( (nb_of_pages) * PAGE_LENGTH ) ;

    return;
}

int compute_merkle(FILE **fp, merkle_tree *mt, char **result) {
    //File does not exist
    if (*fp == NULL) {
        log_msg("%s\n", "ERROR : File does not exist");
        return -1;
    }

    int file_size = get_file_size(fp);

    //Reads file into file_string
    char *file_string = malloc(file_size + 1);
    fread(file_string, file_size, 1, *fp);

    //Compute array
    int page_full      = is_full(file_size);
    int nb_of_pages    = (int) file_size / PAGE_LENGTH;
    int tree_size      = compute_tree_size(nb_of_pages + 1 - page_full);

    //Memory allocation
    char **parsed_file = calloc(tree_size, sizeof(char **) * sizeof(char *) * PAGE_LENGTH );
    *result = malloc(sizeof(char *) * HASH_SIZE * 2 * tree_size);

    for (int i = 0; i < tree_size ; i++){
      *parsed_file = calloc(PAGE_LENGTH, sizeof(char *));
      parsed_file += PAGE_LENGTH;
    }
    parsed_file = parsed_file - tree_size*PAGE_LENGTH;

    //File parsing
    parse_file(parsed_file, &file_string, nb_of_pages, tree_size, page_full);

    //Assigning values for merkle_tree calculation
    int height = (int) log2(tree_size) + 1;
    mt->tree_height = height;
    mt->nb_nodes    = 0;
    mt->nb_of_leaves = tree_size;

    if (build_tree(mt, parsed_file) == -1) {
        log_msg("%s\n", "ERROR : Tree could not build");
        return -1;
    }

    //Stores result in a string
    tree_to_string(mt, *result);

    //Freeing memory
    free_merkle(mt);
    free_file(parsed_file,nb_of_pages, page_full);
    file_string -= ((nb_of_pages) * PAGE_LENGTH);
    free(parsed_file);
    free(file_string);

    return 0;
}

int m_compute_merkle(FILE **fp, merkle_tree *mt, char **result, int nb_threads) {
    //File does not exist
    if (*fp == NULL)
        return -1;

    int file_size = get_file_size(fp);

    //Reads file into file_string
    char *file_string = malloc(file_size + 1);
    fread(file_string, file_size, 1, *fp);

    //Compute array
    int page_full = is_full(file_size);
    int nb_of_pages = (int) file_size / PAGE_LENGTH;

    //If there is less pages than threads, we call the single-threaded method
    if (nb_of_pages < nb_threads) {
        return compute_merkle(fp, mt, result);
    }

    int tree_size = compute_tree_size(nb_of_pages + 1 - page_full);

    //Memory allocation
    char **parsed_file = calloc(tree_size, sizeof(char **) * sizeof(char *) * PAGE_LENGTH );
    *result            = malloc(sizeof(char *) * HASH_SIZE * 2 * tree_size);

    for (int i = 0; i < tree_size ; i++){
      *parsed_file = calloc(PAGE_LENGTH, sizeof(char *));
      parsed_file += PAGE_LENGTH;
    }
    parsed_file = parsed_file - tree_size*PAGE_LENGTH;

    //File parsing
    parse_file(parsed_file, &file_string, nb_of_pages, tree_size, page_full);

    //Assigning values for merkle_tree calculation
    int height = (int) log2(tree_size) + 1;
    mt->tree_height = height;
    mt->nb_nodes    = 0;
    mt->nb_of_leaves = tree_size;

    if (m_build_tree(mt, parsed_file, nb_threads) == -1) {
        log_msg("%s\n", "ERROR : Tree could not build.");
        return -1;
    }

    //Stores result in a string
    tree_to_string(mt, *result);

    //Freeing memory
    free_merkle(mt);
    free_file(parsed_file,nb_of_pages, page_full);
    file_string -= ((nb_of_pages) * PAGE_LENGTH);
    free(parsed_file);
    free(file_string);

    return 0;
}

int pages_in_need(int size, int offset, merkle_tree *mt, FILE **fp, char **result) {

    //File does not exist
    if (*fp == NULL) {
        log_msg("%s\n", "ERROR : File does not exist");
        return -1;
    }

    fseek(*fp, 0L, SEEK_END);
    int file_size = ftell(*fp);
    fseek(*fp, 0L, SEEK_SET);

    //Reads file into file_string
    char *file_string = malloc(file_size + 1);
    fread(file_string, file_size, 1, *fp);

    int nb_of_pages = (int) file_size / PAGE_LENGTH;
    //If the new file has more pages than the tree can contain, compute a new one
    if (nb_of_pages > (mt->nb_nodes / 2))
        return compute_merkle(fp, mt, result);


    int first_page = (int) offset / PAGE_LENGTH;
    int last_page  = (int) (offset + size) / PAGE_LENGTH;

    if (is_full(offset + size))
        last_page--;

    int number           = last_page - first_page + 1;
    int leaf_start_index = (1 << (mt->tree_height - 1));

    int indexes[number];
    for (int i = 0 ; i < number ; i ++) {
          indexes[i] = leaf_start_index + first_page + i;
    }

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
        new_datas   += PAGE_LENGTH;
        file_string += PAGE_LENGTH;
    }

    //We consider the case the last page is concernerned
    strlcpy(*new_datas, file_string, PAGE_LENGTH);

    //Pointers back at initial value
    new_datas   -= ((number-1) * PAGE_LENGTH);
    file_string -= ((first_page + number - 1) * PAGE_LENGTH);

    if (change_and_rebuild(mt, indexes, new_datas, number, FILE_MODE) == -1)
        return -1;

    *result = malloc(HASH_SIZE * mt->nb_nodes);
    tree_to_string(mt, *result);

    //Freeing memory
    free(*new_datas);
    free(new_datas);
    free(file_string);
    free_merkle(mt);

    return 0;

}

int quick_change(int size, int offset, char **buf, merkle_tree *mt, char ** result) {

    if (is_full(offset)) {

      int page_to_change = (int) offset / PAGE_LENGTH;
      int leaf_start_index = (1 << (mt->tree_height - 1));

      if (page_to_change + 1 > (mt->nb_nodes / 2)) {
          merkle_tree new_mt;
          get_bigger_tree(mt, &new_mt);
          int indexes[1];
          int new_leaf_start_index = (1 << (new_mt.tree_height - 1));
          indexes[0] = new_leaf_start_index + page_to_change;

          if (change_and_rebuild(&new_mt, indexes, buf, 1, BUFFER_MODE) == -1)
              return -1;

          *result = malloc(sizeof(char *) * HASH_SIZE * 2 * new_mt.nb_nodes);
          tree_to_string(&new_mt, *result);
          log_msg("%s\n", "Managed to get here");
          free_merkle(mt);
          free_merkle(&new_mt);
          return 0;

      }

      int indexes[1];
      indexes[0] = leaf_start_index + page_to_change;

      if (change_and_rebuild(mt, indexes, buf, 1, BUFFER_MODE) == -1)
          return -1;

      *result = malloc(sizeof(char *) * HASH_SIZE * 2 * mt->nb_nodes);
      tree_to_string(mt, *result);

      free_merkle(mt);

      return 0;

    }

    else {
        return -2;
    }
}

int root_calculation(merkle_tree *mt, char **result) {

    int leaf_start_index = (1 << (mt->tree_height - 1));
    for (int i = leaf_start_index-1; i > 0 ; i--){
        mt->nodes[i].hash = NULL;
        if (hash_node(mt, i) == -1)
            return -1;
    }
    *result = malloc(sizeof(char *) * HASH_SIZE * 2 * leaf_start_index);
    tree_to_string(mt, *result);
    free_merkle(mt);
    return 0;
}

int get_bigger_tree(merkle_tree *old_mt, merkle_tree *new_mt) {

    new_mt->tree_height = 1 + old_mt->tree_height;
    new_mt->nb_nodes = (1 << (new_mt->tree_height));

    int old_start_index = (1 << (old_mt->tree_height - 1));
    int new_start_index = (1 << (new_mt->tree_height - 1));

    new_mt->nodes    = malloc(sizeof(node) * (new_mt->nb_nodes + 1) * 2 * PAGE_LENGTH * HASH_SIZE);

    for (int i = 0; i < old_mt->nb_nodes - old_start_index ; i++){
      new_mt->nodes[new_start_index + i].hash = malloc(sizeof(char *)* strlen(old_mt->nodes[old_start_index + i].hash));
      strcpy(new_mt->nodes[new_start_index + i].hash, old_mt->nodes[old_start_index + i].hash);
    }

    for (int i = new_start_index + old_mt->nb_nodes - old_start_index; i < new_mt->nb_nodes ; i++){
        new_mt->nodes[i].data = malloc(sizeof(char *)* strlen(" "));
        strcpy(new_mt->nodes[i].data, " ");
        new_mt->nodes[i].hash = NULL;
        if (hash_node(new_mt, i) == -1)
          return -1;
    }

    for (int i = new_start_index - 1; i > 0; i--) {
        //mt->nodes[i].hash = NULL;
        new_mt->nodes[i].hash = malloc(sizeof(char *)* strlen("deprecated"));
        strcpy(new_mt->nodes[i].hash, "deprecated");
        // if (hash_node(mt, i) == -1)
        //     return -1;

    }

    return 0;
}
