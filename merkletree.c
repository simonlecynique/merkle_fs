#include "merkletree.h"

int build_tree(merkle_tree *mt, char **data_table) {
    int leaf_start_index = (1 << (mt->tree_height - 1));
    mt->nb_nodes = (1 << (mt->tree_height));
    mt->nodes = (node *)malloc(sizeof(node) * (mt->nb_nodes + 1));
    for (int i = leaf_start_index; i< mt->nb_nodes; i++) {
        mt->nodes[i].data = data_table[i-leaf_start_index];
        mt->nodes[i].hash = NULL;
        hash_node(mt, i);
    }
    for (int i = leaf_start_index - 1; i>0; i--) {
        mt->nodes[i].hash = NULL;
        hash_node(mt, i);
    }
    return 0;
}

int hash_node(merkle_tree *mt, int i) {
    //If the index is out of the range, return.
    if (i > mt->nb_nodes - 1 )
        return -1;

    sha3_context c;
    uint8_t *hash;
    char string[4];
    char hash_string[4*32];
    hash_string[0] = '\0';

    //The node considered is not a leaf.
    if (i < (1 << (mt->tree_height-1))) {
        if (mt->nodes[2*i].hash && mt->nodes[2*i+1].hash) {
            char *buffer = (char *)malloc(sizeof(char *) * HASH_SIZE / 4);
            memcpy(buffer, mt->nodes[2*i].hash, HASH_SIZE / 8);
            memcpy(buffer + (HASH_SIZE / 8), mt->nodes[2*i+1].hash, HASH_SIZE / 8);

            //We hash the concatenation of the two related nodes.
            mt->nodes[i].hash = (char *)malloc(sizeof(char *) * HASH_SIZE / 8);
            sha3_Init256(&c);
            sha3_Update(&c, buffer, strlen(buffer));
            hash = (uint8_t *) sha3_Finalize(&c);
            for (int j=0 ; j < 32 ; j++) {
                sprintf(string, "%x", hash[j]);
                strcat(hash_string, string);
            }
            strcpy(mt->nodes[i].hash, hash_string);
        }
    }
    //endif

    //Else, the node is a leaf.
    else {
        if (mt->nodes[i].data) {
            mt->nodes[i].hash = (char *)malloc(sizeof(char *) * HASH_SIZE / 4);
            sha3_Init256(&c);
            sha3_Update(&c, mt->nodes[i].data, strlen(mt->nodes[i].data));
            hash = (uint8_t *)sha3_Finalize(&c);
            for (int j=0 ; j < 32 ; j++) {
                sprintf(string, "%x", hash[j]);
                strcat(hash_string, string);
            }
            strcpy(mt->nodes[i].hash, hash_string);
        }
    }
    return 0;
}

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

int set_tree_data(merkle_tree *mt, int i, char *data) {
    mt->nodes[i].data = data;
    int j = i;
    //Recomputes all the changed nodes after change.
    while (j) {
        hash_node(mt, j);
        j = j / 2;
    }
    return 0;
}

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

//Format : "1:hash,2:hash"
void tree_to_string(merkle_tree *mt, char tree[]) {

    char tree_string[32 * 4 * mt->nb_nodes];
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

    char* *tokens;
    char* *thing;
    tokens = str_split(tree_string, ',');

    if (tokens)
    {
        int nb_nodes;
        for (nb_nodes = 0; *(tokens + nb_nodes); nb_nodes++) {}
        mt->nb_nodes = nb_nodes + 1;
        mt->tree_height = (int) log2(nb_nodes + 1);

        for (int i = 0; *(tokens + i); i++)
        {
            mt->nodes[i + 1].hash = (char *) malloc(sizeof(char)*HASH_SIZE/4);
            thing = str_split(*(tokens + i), ':');
            char *hash_string = *(thing + 1);
            strcpy(mt->nodes[i + 1].hash, hash_string);
            //free allocated char *.
            free(*(tokens + i));
            free(thing);
        }
        free(tokens);
    }
}

void freeMerkleTree(merkle_tree *mt) {

    int i;
    if (!mt)
        return;
    if (mt->nodes) {
        for (i=1; i<=mt->nb_nodes; i++)
            if(mt->nodes[i].hash)
                free(mt->nodes[i].hash);
        free(mt->nodes);
    }
    return;
}

// #define TREE_HEIGHT 4
// #define BLOCKS 8
// int main(int argc, char const *argv[]) {
//
//     //---------CREATING MERKLE TREE A
//     merkle_tree mt_a = {0, TREE_HEIGHT, BLOCKS, NULL};
//     char *data_table[BLOCKS];
//     for (int i = 0 ; i < BLOCKS ; i++ ) {
//       data_table[i] = "hello";
//     }
//     build_tree(&mt_a, data_table);
//     //printf("%s\n", mt_a.nodes[1].hash );
//     //print_tree(&mt_a);
//
//     //---------CREATING MERKLE TREE B
//     //merkle_tree mt_b = {0, TREE_HEIGHT, BLOCKS, NULL};
//     //char *data_tab[BLOCKS];
//     //for (int i = 0 ; i < BLOCKS-1 ; i++ ) {
//     //  data_tab[i] = "hello";
//     //}
//     //data_tab[BLOCKS-1] = "hellow";
//     //build_tree(&mt_b, data_tab);
//     //print_tree(&mt_b);
//
//     //---------COMPARES A AND B
//     //compare_trees(&mt_a, &mt_b, 1);
//
//     //---------TRANSFORMS A TO STRING
//     char resu[32 * 4 * mt_a.nb_nodes];
//     tree_to_string(&mt_a, resu);
//     printf("%s\n", resu );
//     merkle_tree mt_c;
//     string_to_tree(&mt_c, resu);
//     compare_trees(&mt_a, &mt_c, 1);
//     char resu2[32 * 4 * mt_a.nb_nodes];
//     tree_to_string(&mt_c, resu2);
//     printf("%s\n", resu2 );
//     return 0;
// }
