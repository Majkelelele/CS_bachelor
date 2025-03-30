#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <endian.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "sum-common.h"
#include "err.h"
#include "common.h"
// #include "clients_and_servers.h"
#include "server_TCP.c"
#include "server_UDP.c"

#define QUEUE_LENGTH  5
#define SOCK_TIMEOUT  20

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <protocol_ID> <port>", argv[0]);
    }
    
    enum ProtocolIdentifier protocol_id = get_identifier(argv[1]);
    uint16_t port = read_port(argv[2]);
    if(protocol_id == TCP_id) run_TCP_server(port);
    if(protocol_id == UDP_id) run_UDP_server(port);
    else {
        syserr("wrong protocol ID: %d", protocol_id);
        return -1;
    }
    
    return 0;
}
