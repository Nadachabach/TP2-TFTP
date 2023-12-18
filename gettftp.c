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
    // Check if the correct number of command-line arguments is provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Extract command-line arguments
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
    int status = getaddrinfo(host, "1069", &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // Debugging: Print the list of addresses
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
        // Attempt to create a socket
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            continue; // Error, try the next address
        }

        break; // Success, we have a socket
    }

    // Check if no address was usable
    if (rp == NULL) {
        fprintf(stderr, "Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    // Build the RRQ (Read Request) packet
    char rrqPacket[516];  // The maximum size of a TFTP packet is 516 bytes

    // Opcode (RRQ)
    uint16_t opcode = htons(1);  // Opcode 1 for RRQ
    memcpy(rrqPacket, &opcode, sizeof(uint16_t));

    // Filename
    strcpy(rrqPacket + 2, filename);

    // Null character after the filename
    int filenameLength = strlen(filename);
    rrqPacket[2 + filenameLength] = '\0';

    // Mode (octet)
    const char* mode = "octet";
    strcpy(rrqPacket + 2 + filenameLength + 1, mode);

    // Null character after the mode
    int modeLength = strlen(mode);
    rrqPacket[2 + filenameLength + 1 + modeLength] = '\0';

    // Send the RRQ packet to the server
    ssize_t bytesSent = sendto(sockfd, rrqPacket, 2 + filenameLength + 1 + modeLength + 1, 0, rp->ai_addr, rp->ai_addrlen);
    if (bytesSent == -1) {
        error("Error sending RRQ packet");
    }

    // Clean up: free the list of addresses and close the socket
    freeaddrinfo(result);
    close(sockfd);

    return 0;
}

