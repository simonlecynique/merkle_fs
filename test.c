#include "parse.h"

//Tests the function of parse.c. Prints the Merkle Tree of parse.h file.

int parse_test(void) {

    merkle_tree mt_a;
    FILE *fp = fopen("parse.h", "r");
    char* result;

    parse_file(&fp, &mt_a, &result);
    printf("%s\n", result );
    return 0;
}

//Tests Merkle Trees by computing two different ones and comparing them.
//tree_to_string is not tested here because used in parse_test.

#define TREE_HEIGHT 4
#define BLOCKS 8

int test_merkle_tree(void) {

    //---------CREATING MERKLE TREE A
    // merkle_tree mt_a = {0, TREE_HEIGHT, BLOCKS, NULL};
    // char **data_table = (char **) malloc(sizeof(char *) * BLOCKS * strlen("hello"));
    // for (int i = 0 ; i < BLOCKS ; i++ ) {
    //   *data_table = "hello";
    //   data_table += strlen("hello");
    // }
    // data_table -= BLOCKS * strlen("hello");
    // build_tree(&mt_a, data_table);
    // printf("%s\n", "Printing Merkle Tree A :" );
    // print_tree(&mt_a);

    //---------CREATING MERKLE TREE B
    merkle_tree mt_b = {0, TREE_HEIGHT, BLOCKS, NULL};
    char **data_tab = (char **) malloc(sizeof(char *) * BLOCKS * strlen("hello"));
    for (int i = 0 ; i < BLOCKS ; i++ ) {
      *data_tab = "hello";
      data_tab += strlen("hello");
    }
    data_tab -= BLOCKS * strlen("hello");
    build_tree(&mt_b, data_tab);
    printf("%s\n", "Printing Merkle Tree B :" );
    print_tree(&mt_b);

    char **changes = (char **) malloc(sizeof(char *) * 2 * strlen("hello"));
    *changes = "putes";
    changes += strlen("hello");
    *changes = "negres";
    changes -= strlen("hello");
    int number = 2;
    int indexes[number];
    indexes[0] = 11;
    indexes[1] = 14;
    change_tree_data(&mt_b, indexes, changes, number);
    printf("%s\n", "Printing changed Merkle Tree B :" );
    print_tree(&mt_b);


    //---------COMPARES A AND B
    //compare_trees(&mt_a, &mt_b, 1);

    return 0;
}

int main(void) {
    FILE *fp = fopen("parse.h", "r");
    //parse_test();
    test_merkle_tree();
    merkle_tree mt_a;
    char* result;
    compute_merkle(&fp, &mt_a, &result);


    return 0;
}
