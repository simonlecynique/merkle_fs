#include "merklelog.h"

void merkle_log(char *message, char* fpath) {
    FILE *file;

    file = fopen(LOGFILE, "a");
    fputs(fpath, file);
    fputs(" :\n", file);
    fputs(message, file);
    fputs("\n");

    fclose(file);

}
