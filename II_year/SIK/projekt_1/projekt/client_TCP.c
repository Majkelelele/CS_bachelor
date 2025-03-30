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
#include "protconst.h"
// #include "clients_and_servers.h"


void run_TCP_client(char const *host, uint16_t port) {
    enum ProtocolIdentifier id = TCP_id;
    struct sockaddr_in server_address = get_server_address(host, port);
    int size = 0;
    char *input = read_input(&size);
    // Create a socket.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    // Connect to the server.
    if (connect(socket_fd, (struct sockaddr *) &server_address,
                (socklen_t) sizeof(server_address)) < 0) {
        syserr("cannot connect to the server");
    }
    struct timeval to = {.tv_sec = MAX_WAIT, .tv_usec = 0};
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof to);
    setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof to);

    CONN conn = create_CONN(id,size);

    if(send_CONN_TCP(socket_fd,&conn) == -1) {
        free(input);
        close(socket_fd);
        return;
    }
    CONACC conacc;
    if(read_CONACC_TCP(socket_fd,&conacc,conn.session_id) == -1) {
        close(socket_fd);
        free(input);
        return;
    }

    DATA data_to_send;
    for(int i = 0; size > 0; i++) {
        int packet_data_size = min(size,MAX_PACKET_SIZE);

        data_to_send.data = input + i*sizeof(char)*MAX_PACKET_SIZE;
        data_to_send.data_byte_count = packet_data_size*sizeof(char);
        data_to_send.packet_number = i;
        data_to_send.packet_type = DATA_id;
        data_to_send.session_id = conn.session_id;
        ssize_t send_size = send_DATA_TCP(socket_fd,&data_to_send);
        if(send_size == -1) {
            free(input);
            close(socket_fd);
            return;
        }
        size -= send_size;
    }
    free(input);

    RCVD received;
    if(read_RCVD_TCP(socket_fd,&received,conn.session_id) == -1) {
        close(socket_fd);
        return;
    }
    
    close(socket_fd);
}