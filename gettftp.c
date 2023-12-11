#include <stdio.h>
#include <stdlib.h>

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *host = argv[1];
    const char *filename = argv[2];

    printf("Host: %s\n", host);
    printf("File: %s\n", filename);

   

    return 0;
}
