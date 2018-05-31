#include "merkletree.h"

//SINGLE THREADED---------------------------------------------------------------

int build_tree(merkle_tree *mt, char **datas) {

    int data_size = 0;

    //Setting values of Merkle Tree
    int leaf_start_index = (1 << (mt->tree_height - 1));
    mt->nb_nodes = (1 << (mt->tree_height));
    mt->nodes    = malloc(sizeof(node) * (mt->nb_nodes + 1) * 2 * PAGE_LENGTH * HASH_SIZE);

    //Hashing leaves
    for (int i = leaf_start_index; i < mt->nb_nodes; i++) {
        mt->nodes[i].data = malloc(sizeof(char *) * 2 * PAGE_LENGTH);

        //If there is a problem with data, send error
        if (*(datas + data_size) == NULL) {
            log_msg("%s\n", "ERROR in build_tree: NULL somewhere in data");
            return -1;
        }

        strlcpy(mt->nodes[i].data, *(datas + data_size), 1 + strlen(*(datas + data_size)));
        data_size += strlen(*(datas + data_size));
        mt->nodes[i].hash = NULL;

        if (hash_node(mt, i) == -1)
            return -1;

    }

    //Hashing others
    for (int i = leaf_start_index - 1; i > 0; i--) {
        mt->nodes[i].hash = NULL;

        if (hash_node(mt, i) == -1)
            return -1;

    }

    return 0;
}

int hash_node(merkle_tree *mt, int i) {
    //If the index is out of the range, return.
    if (i > mt->nb_nodes - 1 )
        return -1;

    //Creating variables
    sha3_context c;
    uint8_t *hash;
    char string[HEX];
    char hash_string[HEX * HASH_SIZE / BYTE_SIZE];
    hash_string[0] = '\0';


    //The node considered is not a leaf.
    if (i < (1 << (mt->tree_height-1))) {
        //The two children leaves have a hash computed
        if (mt->nodes[2*i].hash && mt->nodes[2*i+1].hash) {
            //Memory allocation
            char *buffer = malloc(sizeof(char *) * 2 * HASH_SIZE / BYTE_SIZE);
            memcpy(buffer, mt->nodes[2*i].hash, HASH_SIZE / BYTE_SIZE);
            memcpy(buffer + (HASH_SIZE / 8), mt->nodes[2*i+1].hash, HASH_SIZE / BYTE_SIZE);

            //We hash the concatenation of the two related nodes.
            mt->nodes[i].hash = malloc(sizeof(char *) * HASH_SIZE / BYTE_SIZE);
            sha3_Init256(&c);
            sha3_Update(&c, buffer, strlen(buffer));
            hash = (uint8_t *) sha3_Finalize(&c);
            for (int j=0 ; j < (HASH_SIZE / BYTE_SIZE) ; j++) {
                sprintf(string, "%x", hash[j]);
                strcat(hash_string, string);
            }
            strcpy(mt->nodes[i].hash, hash_string);
            free(buffer);
        }

        //If they don't, return an error
        else {
            return -1;
        }

    }

    //Else, the node is a leaf.
    else {
        //Checks if the leaf has data to hash
        if (mt->nodes[i].data) {
            mt->nodes[i].hash = malloc(sizeof(char *) * 2 * HASH_SIZE / BYTE_SIZE);
            sha3_Init256(&c);
            sha3_Update(&c, mt->nodes[i].data, strlen(mt->nodes[i].data));
            hash = (uint8_t *)sha3_Finalize(&c);
            for (int j=0 ; j < (HASH_SIZE / BYTE_SIZE) ; j++) {
                sprintf(string, "%x", hash[j]);
                strcat(hash_string, string);
            }
            strcpy(mt->nodes[i].hash, hash_string);
        }
        //If it does not, return an error
        else {
            return -1;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------

//MULTI THREADED----------------------------------------------------------------

int set_tree_datas(merkle_tree *mt, char **datas) {

    int total_size = 0;
    //Setting values of Merkle Tree
    int leaf_start_index = (1 << (mt->tree_height - 1));
    mt->nb_nodes         = (1 << (mt->tree_height));
    mt->nodes            = malloc(sizeof(node) * (mt->nb_nodes + 1) * 2 * PAGE_LENGTH * HASH_SIZE);
    //Setting tree datas
    for (int i = leaf_start_index; i < mt->nb_nodes; i++) {
        mt->nodes[i].data = malloc(sizeof(char *) * 2 * PAGE_LENGTH);

        //If there is a problem with data, send error
        if (*(datas + total_size) == NULL) {
            log_msg("%s\n", "ERROR in set_tree_datas: NULL somewhere in data");
            return -1;
        }

        strlcpy(mt->nodes[i].data, *(datas + total_size), 1 + strlen(*(datas + total_size)));
        total_size       += strlen(*(datas + total_size));
        mt->nodes[i].hash = NULL;
    }

    return 0;

}

int m_build_tree(merkle_tree *mt, char **datas, int nb_of_threads) {

    int leaf_start_index = (1 << (mt->tree_height - 1));

    if (set_tree_datas(mt, datas) == -1)
        return -1;

    int nb_leaves_per_thread = (int) leaf_start_index / nb_of_threads ;
    int start_index;
    pthread_t threads[nb_of_threads];
    threaded_arg t_arg[nb_of_threads];

    int new_start_index = leaf_start_index / (1 << ((int)log2(nb_leaves_per_thread) - 1));

    for (int i = 0 ; i < nb_of_threads - 1 ; i ++) {
        //creates thread that computes the hashes
        start_index          = leaf_start_index + (i * nb_leaves_per_thread);
        t_arg[i].mt          = mt;
        t_arg[i].start_index = start_index;
        t_arg[i].nb_leaves   = nb_leaves_per_thread;

        if (pthread_create(&threads[i], NULL, &m_hash_nodes, (void *)&t_arg[i]) != 0)
            return -1;

    }

    //The last thread potentially has to compute more than nb_leaves_per_thread
    start_index                          = leaf_start_index + (nb_of_threads - 1) * nb_leaves_per_thread ;
    t_arg[nb_of_threads - 1].mt          = mt ;
    t_arg[nb_of_threads - 1].start_index = start_index;
    t_arg[nb_of_threads - 1].nb_leaves   = nb_leaves_per_thread + (leaf_start_index % nb_of_threads);

    if (pthread_create(&threads[nb_of_threads - 1], NULL, &m_hash_nodes, (void *)&t_arg[nb_of_threads - 1]) != 0)
        return -1;

    //Waits for every thread to be done
    for (int i = 0 ; i < nb_of_threads ; i ++) {
        pthread_join(threads[i], NULL);
    }

    //Hashing root nodes
    for (int i = new_start_index ; i > 0; i--) {
        mt->nodes[i].hash = NULL;
        if (hash_node(mt, i) == -1)
            return -1;
    }

    return 0;
}

void *m_hash_nodes(void *arg) {

    //Getting arguments through struct
    threaded_arg *t_arg = (threaded_arg *)arg;
    int start_index     = t_arg->start_index;
    int nb_leaves       = t_arg->nb_leaves;
    merkle_tree *mt     = t_arg->mt;

    int nb_level = log2(nb_leaves);

    for (int i = 0 ; i < nb_level ; i ++) {
        for (int j = start_index ; j < start_index + nb_leaves ; j++ ) {
              if (hash_node(mt, j) == -1)
                  log_msg("%s %d\n", "Problem with node number", j );
        }
        start_index = start_index / 2 ;
        nb_leaves   = nb_leaves / 2 ;
    }

    return NULL;
}

//------------------------------------------------------------------------------

// STRING FORMATTING------------------------------------------------------------

//Prints hash of index i node.
void print_hash(merkle_tree *mt, int i) {
    char *hash = mt->nodes[i].hash;
    printf("%s", hash );
}

//Prints whole tree : index, then hash.
void print_tree(merkle_tree *mt) {
    for (int i = 1; i < mt->nb_nodes ; i++ ) {
        printf("%d\n", i);
        print_hash(mt, i);
        printf("\n");
    }
}

//Format : "1:hash,2:hash"
void tree_to_string(merkle_tree *mt, char tree[]) {

    char *tree_string = malloc(HEX * (HASH_SIZE / BYTE_SIZE) * mt->nb_nodes);
    char *hash;
    char number_string[10];
    tree_string[0] = '\0';

    for (int i = 1 ; i < mt->nb_nodes ; i++ ) {
        sprintf(number_string, "%d", i);
        strcat(tree_string, number_string);
        strcat(tree_string, ":");
        hash = mt->nodes[i].hash;
        strcat(tree_string, hash);
        if (i < mt->nb_nodes-1)
            strcat(tree_string, ",");
    }
    strcpy(tree, tree_string);
    free(tree_string);

}

char** str_split(char* a_str, const char a_delim) {

    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    count++;
    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
}

void string_to_tree(merkle_tree *mt, char *tree_string) {

    char **tokens;
    char **thing;
    tokens = str_split(tree_string, ',');
    if (tokens)
    {
        int nb_nodes;
        for (nb_nodes = 0; *(tokens + nb_nodes); nb_nodes++) {}

        mt->nb_nodes    = nb_nodes + 1;
        mt->tree_height = (int) log2(nb_nodes + 1);
        mt->nodes       = malloc(sizeof(node) * (mt->nb_nodes + 1) * 2 * PAGE_LENGTH * HASH_SIZE);

        for (int i = 0; *(tokens + i); i++)
        {
            mt->nodes[i + 1].hash = malloc(sizeof(char) * HEX * HASH_SIZE / BYTE_SIZE);
            thing                 = str_split(*(tokens + i), ':');
            char *hash_string     = *(thing + 1);
            strcpy(mt->nodes[i + 1].hash, hash_string);
        }
        free(tokens);
    }
}

//------------------------------------------------------------------------------

// COMPARISONS AND DATA CHANGES-------------------------------------------------

int change_and_rebuild(merkle_tree *mt, int indexes[], char **datas, int number, int mode) {

    int index;
    int data_size = 0;
    if (mode == FILE_MODE) {
        for (int i = 0; i < number; i ++) {
            index                 = indexes[i];
            mt->nodes[index].data = *(datas + data_size);
            data_size            += strlen(*(datas + data_size));
        }

        int k  = number;
        index = indexes[0];

        int tree_height = mt->tree_height;

        for (int i = 0 ; i < tree_height ; i ++) {
            for (int j = 0 ; j < k ; j ++) {
              if (hash_node(mt, index) == -1)
                  return -1;
              index++;
            }
            index = index - k;
            index = index / 2;
            k     = (k / 2) + 1;
        }
        if (hash_node(mt, 1) == -1)
            return -1;
    }

    else {
      for (int i = 0; i < number; i ++) {
          index                 = indexes[i];
          mt->nodes[index].data = malloc(sizeof(char *) * 2 * PAGE_LENGTH);
          strlcpy(mt->nodes[index].data, *(datas + data_size), 1 + strlen(*(datas + data_size)));
          data_size            += strlen(*(datas + data_size));
      }
      while (index !=0){
          mt->nodes[index].hash = NULL;
          if (hash_node(mt, index) == -1)
            return -1;
          index = index / 2;
      }
    }

    return 0;
}

//To compare whole tree, call compare_trees(a,b, 1);
void compare_trees(merkle_tree *mt_a, merkle_tree *mt_b, int index) {

    if (strcmp(mt_a->nodes[index].hash, mt_b->nodes[index].hash)) {
        printf("%s", "Difference on node number ");
        printf("%d\n", index);

        //If there is a difference and the node is not a leaf, check nodes below.
        if (index < (1 << (mt_a->tree_height - 1)) ) {
            compare_trees(mt_a, mt_b, 2*index);
            compare_trees(mt_a, mt_b, 2*index+1);
        }

    }
}

//------------------------------------------------------------------------------

void free_merkle(merkle_tree *mt) {

    for (int i = 1 ; i < mt->nb_nodes ; i ++) {

        if (i > mt->nb_nodes / 2)
            free(mt->nodes[i].data);
        if (mt->nodes[i].hash)
            free(mt->nodes[i].hash);
    }

    free(mt->nodes);

    return;
}
