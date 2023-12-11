#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    // here we are checking if the correct number of command-line arguments is provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Extracting command-line arguments
    const char *host = argv[1];
    const char *filename = argv[2];

    // Print information about the host and file
    printf("Host: %s\n", host);
    printf("File: %s\n", filename);

    // Get server address from hostname
    struct addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;      // Use IPv4
    hints.ai_socktype = SOCK_DGRAM; // Use UDP

    // Use getaddrinfo to obtain a list of addresses associated with the host
    int status = getaddrinfo(host, "69", &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // Printing the list of addresses
    printf("List of addresses:\n");
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        char addrstr[INET_ADDRSTRLEN];
        struct sockaddr_in *addr = (struct sockaddr_in *)rp->ai_addr;
        inet_ntop(AF_INET, &(addr->sin_addr), addrstr, INET_ADDRSTRLEN);
        printf("  %s\n", addrstr);
    }

    // Iterate over the list of addresses and choose the first one
    int sockfd;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        // creating a socket
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            continue; // Error, try the next address
        }

        break; // Success, we have a socket
    }


    
    freeaddrinfo(result);
    close(sockfd);

    return 0;
}

