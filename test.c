#include "parse.h"

//Tests the function of parse.c. Prints the Merkle Tree of parse.h file.

int parse_test(void) {

    merkle_tree mt_a;
    FILE *fp = fopen("text_file.txt", "r");
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
    merkle_tree mt_a = {0, TREE_HEIGHT, BLOCKS, NULL};
    char *data_table[BLOCKS];
    for (int i = 0 ; i < BLOCKS ; i++ ) {
      data_table[i] = "hello";
    }
    build_tree(&mt_a, data_table);
    printf("%s\n", "Printing Merkle Tree A :" );
    print_tree(&mt_a);

    //---------CREATING MERKLE TREE B
    merkle_tree mt_b = {0, TREE_HEIGHT, BLOCKS, NULL};
    char *data_tab[BLOCKS];
    for (int i = 0 ; i < BLOCKS-1 ; i++ ) {
     data_tab[i] = "hello";
    }
    data_tab[BLOCKS-1] = "hellow";
    build_tree(&mt_b, data_tab);


    //---------COMPARES A AND B
    compare_trees(&mt_a, &mt_b, 1);

    return 0;
}

int main(void) {

    parse_test();
    //test_merkle_tree();

    return 0;
}
