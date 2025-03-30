#include <endian.h>
#include <inttypes.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "sum-common.h"
#include "err.h"
#include "common.h"
// #include "clients_and_servers.h"
#include "client_TCP.c"
#include "client_UDP.c"



int main(int argc, char *argv[]) {
    if (argc != 4) {
        fatal("usage: %s <protocol_ID> <host> <port>\n", argv[0]);
    }

    enum ProtocolIdentifier id = get_identifier(argv[1]);
    if(id == 0) {
        fprintf(stderr, "ERROR: wrong protocol Identifier\n");
        return -1;
    } 
    char const *host = argv[2];
    uint16_t port = read_port(argv[3]);
    if(id == TCP_id) run_TCP_client(host,port);
    else run_UDP_client(host,port,id);


    return 0;
}
