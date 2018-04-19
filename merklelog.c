#include "merklelog.h"

void merkle_log(char *message, char* fpath) {
    FILE *file;

    file = fopen(LOGFILE, "w");
    fputs(fpath, file);
    fputs(" :\n", file);
    fputs(message, file);
    fputs("\n", file);

    fclose(file);

}
