#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>   
#include <unistd.h>   
#include <string.h>

#define PATH_MAX 4096
#define DREXEL_DRAGON "dragon.txt"


// EXTRA CREDIT - print the drexel dragon from the readme.md
extern void print_dragon(){
    char exe_path[PATH_MAX];
    ssize_t len;

    len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    exe_path[len] = '\0'; 

    char *dir = dirname(exe_path);

    strcat(dir, "/");
    strcat(dir, DREXEL_DRAGON);


    FILE *file = fopen(dir, "r");
    if (file == NULL) {
        printf("error: failed to open %s: %s\n", DREXEL_DRAGON, dir);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }
    printf("\n");

    fclose(file);
}