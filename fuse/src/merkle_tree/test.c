#include "parse.h"

//Tests the function of parse.c. Prints the Merkle Tree of parse.h file.

int compute_merkle_test(void) {

    FILE *fp = fopen("hello.txt", "r");
    merkle_tree mt_a;
    char* result;

    compute_merkle(&fp, &mt_a, &result);
    printf("%s\n", result );

    merkle_tree mt_b;
    string_to_tree(&mt_b, result);
    char* result2;
    result2 = (char *) malloc(HASH_SIZE * mt_b.nb_nodes);
    tree_to_string(&mt_b, result2);
    printf("%s\n", result2 );

    return 0;
}

int multi_thread_merkle_tree(void) {
    FILE *fp = fopen("text_file.txt", "r");
    merkle_tree mt_a;
    char* result;
    merkle_tree mt_b;

    m_compute_merkle(&fp, &mt_a, &result, 8);
    //printf("%s\n", result );
    // string_to_tree(&mt_b, result);
    // char* result2;
    // result2 = (char *) malloc(HASH_SIZE * mt_b.nb_nodes);
    // tree_to_string(&mt_b, result2);
    // printf("%s\n", result2 );
    return 0;
}

int changes_test() {
    FILE *fp = fopen("text_file.txt", "r");
    merkle_tree mt_a;
    char* result;

    compute_merkle(&fp, &mt_a, &result);
    printf("%s\n", result );

    char* result2;
    pages_in_need(18, 154829, &mt_a, &fp, &result2);
    printf("%s\n", result );
    return 0;
}

//Tests Merkle Trees by computing two different ones and comparing them.
//tree_to_string is not tested here because used in parse_test.

#define TREE_HEIGHT 4
#define BLOCKS 8

// int test_merkle_tree(void) {
//
//     //---------CREATING MERKLE TREE A
//     merkle_tree mt_a = {0, TREE_HEIGHT, BLOCKS, NULL};
//     char **data_table = (char **) malloc(sizeof(char *) * BLOCKS * strlen("hello"));
//     for (int i = 0 ; i < BLOCKS ; i++ ) {
//       *data_table = "hello";
//       data_table += strlen("hello");
//     }
//     data_table -= BLOCKS * strlen("hello");
//     build_tree(&mt_a, data_table);
//     printf("%s\n", "Printing Merkle Tree A :" );
//     print_tree(&mt_a);
//
//     //---------CREATING MERKLE TREE B
//     merkle_tree mt_b = {0, TREE_HEIGHT, BLOCKS, NULL};
//     char **data_tab = (char **) malloc(sizeof(char *) * BLOCKS * strlen("hello"));
//     for (int i = 0 ; i < BLOCKS ; i++ ) {
//       *data_tab = "hello";
//       data_tab += strlen("hello");
//     }
//     data_tab -= BLOCKS * strlen("hello");
//     build_tree(&mt_b, data_tab);
//     printf("%s\n", "Printing Merkle Tree B :" );
//     print_tree(&mt_b);
//
//     char **changes = (char **) malloc(sizeof(char *) * 2 * strlen("hello"));
//     *changes = "putes";
//     changes += strlen("hello");
//     *changes = "negres";
//     changes -= strlen("hello");
//     int number = 2;
//     int indexes[number];
//     indexes[0] = 11;
//     indexes[1] = 14;
//     change_and_rebuild(&mt_b, indexes, changes, number);
//     printf("%s\n", "Printing changed Merkle Tree B :" );
//     print_tree(&mt_b);
//
//
//     //---------COMPARES A AND B
//     compare_trees(&mt_a, &mt_b, 1);
//
//     return 0;
// }

int main(void) {

    //test_merkle_tree();
    //compute_merkle_test();
    multi_thread_merkle_tree();
    //changes_test();

    return 0;
}
