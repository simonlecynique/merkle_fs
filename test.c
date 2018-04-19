#include "parse.h"
#include "merklelog.h"

int main(void) {

    merkle_tree mt_a;
    FILE *fp = fopen("parse.h", "r");
    parse_file(&fp, &mt_a);

    char* result = (char*)malloc(sizeof(char) * 32 * 8 * mt_a.nb_nodes);
    tree_to_string(&mt_a, result);
    merkle_log(result, "parse.h");

    return 0;
}
