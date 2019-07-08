#include "test_fs.h"

char FULL_PAGE[PAGE_LENGTH] =   "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" ;

char *INSERT = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";

int test_fs(int nb_threads, int nb_files_per_threads, int size_of_files, int size_to_write, char *path){

    pthread_t write_threads[nb_threads];
    pthread_t rand_threads[nb_threads];
    arguments args[nb_threads];
    char number_string[10];

    //File Creation.
    for (int i = 0 ; i < nb_threads; i ++) {
        args[i].name = malloc(50);
        strcpy(args[i].name, path);
        strcat(args[i].name, "t");
        sprintf(number_string, "%d", i);
        strcat(args[i].name, number_string);

        args[i].nb_of_files   = nb_files_per_threads;
        args[i].size_of_file  = size_of_files;
        args[i].size_to_write = size_to_write;

        if (pthread_create(&write_threads[i], NULL, &create_files, (void *)&args[i]) != 0)
            return -1;

    }

    for (int i = 0 ; i < nb_threads ; i ++) {
        pthread_join(write_threads[i], NULL);
    }

    //Random writes.
    for (int i = 0; i < nb_threads; i ++){

        if (pthread_create(&rand_threads[i], NULL, &rand_writes, (void *)&args[i]) != 0)
            return -1;

    }

    for (int i = 0 ; i < nb_threads ; i ++) {
        pthread_join(rand_threads[i], NULL);
    }

    return 0;
}

void *create_files(void *arg) {
    arguments *args       = (arguments *)arg;
    char* name            = args->name;
    int nb_of_files       = args->nb_of_files;
    int size_of_file      = args->size_of_file;
    char number_string[10];
    char filename[50];

    for (int i = 0; i < nb_of_files; i++) {
        sprintf(number_string, "%d", i);
        strcpy(filename, name);
        strcat(filename, "f");
        strcat(filename, number_string);
        FILE *fp = fopen(filename, "w");
        for (int j = 0 ; j < size_of_file ; j++){
            fwrite(FULL_PAGE, sizeof(char), strlen(FULL_PAGE), fp);
        }
        fclose(fp);
    }

    return NULL;
}

void *rand_writes(void *arg) {
    arguments *args       = (arguments *)arg;
    char* name            = args->name;
    int nb_of_files       = args->nb_of_files;
    int size_to_write     = args->size_to_write;
    char number_string[10];
    char filename[50];

    for (int i = 0; i < nb_of_files; i++) {
        sprintf(number_string, "%d", i);
        strcpy(filename, name);
        strcat(filename, "f");
        strcat(filename, number_string);
        random_writes(filename, size_to_write);
    }

    return NULL;
}

int random_writes(char *filename, int size_to_write) {

    char * buffer;
    srand(time(NULL));
    int r;
    FILE *fp;
    int position;
    int lSize;

    for (int i = 0; i < size_to_write; i++) {

      fp = fopen(filename, "r+");
      fseek(fp, 0L, SEEK_END);
      int size_of_file = ftell(fp);

      position = rand() % size_of_file ;
      fseek(fp, position, SEEK_SET);

      lSize = size_of_file - position;
      // buffer = (char*) malloc(sizeof(char) * lSize);
      // size_t result = fread(buffer, 1, lSize, fp);

      fseek(fp, position, SEEK_SET);
      fwrite(INSERT,1 , strlen(INSERT), fp);//you should know what you want to write (word) and it is a char pointer
      //fwrite(buffer, 1, strlen(buffer), fp);//fwrite the buffer

      // free(buffer);
      fclose(fp);

      //

    }


    return 0;
}

int delete_files(int nb_threads, int nb_files_per_threads, char *path){

    char thread_number[10];
    char file_number[10];
    char filename[50];
    for (int i = 0; i < nb_threads; i++){
        sprintf(thread_number, "%d", i);
        for (int j = 0; j < nb_files_per_threads; j++) {
            sprintf(file_number, "%d", j);
            strcpy(filename, path);
            strcat(filename, "t");
            strcat(filename, thread_number);
            strcat(filename, "f");
            strcat(filename, file_number);
            remove(filename);
        }
    }

    return 0;
}

int main(int argc, char const *argv[]) {

    time_t start = time(NULL);
    int nb_threads;
    int size_of_files;
    int nb_files_per_threads;
    char *path;

    if (argc < 2) {
        printf("%s\n", "Running default mode");
        nb_threads           = 10;
        nb_files_per_threads = 10;
        size_of_files        = 100;
    }

    else if (argc != 5) {
        printf("%s\n", "Expecting 4 arguments. Usage : nb_threads, nb_files_per_threads, size_of_files, path");
        return 1;
    }

    else {
      nb_threads           = strtol(argv[1], (char **)NULL, 10);
      nb_files_per_threads = strtol(argv[2], (char **)NULL, 10);
      size_of_files        = strtol(argv[3], (char **)NULL, 10);
      path = argv[4];
    }

    if (!test_fs(nb_threads, nb_files_per_threads, size_of_files, 20, "fuse/example/mountdir/"))
        printf("%s %d %s\n", "All", nb_threads * nb_files_per_threads, "files created." );
    //delete_files(nb_threads, nb_files_per_threads, "fuse/example/mountdir/");


    time_t end = time(NULL);

    printf("%ld %s\n", end - start, "seconds ellapsed");
    return 0;
}
