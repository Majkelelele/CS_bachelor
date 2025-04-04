#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <poll.h>
#include <endian.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "common.h"
#include "err.h"
#include "server-func.h"


#define QUEUE_LENGTH 5

int main(int argc, char *argv[]) {
    

    int port = 0;
    std::string file;
    int timeout;

    parseArguments(argc, argv, &port, &file, &timeout);

    parseDealsFromFile(file);
    if (deals.empty()) {
        std::cerr << "Brak rozdań do wyświetlenia lub plik jest pusty.\n";
        return 1;
    }

    // Create a socket.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    // Bind the socket to a concrete address.
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // Listening on all interfaces.
    server_address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        syserr("bind");
    }
    // Switch the socket to listening.
    if (listen(socket_fd, QUEUE_LENGTH) < 0) {
        syserr("listen");
    }

    start_server(socket_fd);
    pthread_barrier_wait(&final_barrier);

    // Zwolnienie pamięci zaalokowanej dla typów rozdań
    for (auto& deal : deals) {
        delete deal.dealType;
    }
    return 0;
}
