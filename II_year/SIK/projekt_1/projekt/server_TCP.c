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
#include "protconst.h"

#define QUEUE_LENGTH  5
#define SOCK_TIMEOUT  20

void run_TCP_server(uint16_t port) {
    // Ignore SIGPIPE signals, so they are delivered as normal errors.
    signal(SIGPIPE, SIG_IGN);

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

    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof server_address) < 0) {
        syserr("bind");
    }

    // Switch the socket to listening.
    if (listen(socket_fd, QUEUE_LENGTH) < 0) {
        syserr("listen");
    }

    // Find out what port the server is actually listening on.
    socklen_t lenght = (socklen_t) sizeof server_address;
    if (getsockname(socket_fd, (struct sockaddr *) &server_address, &lenght) < 0) {
        syserr("getsockname");
    }

    // printf("listening on port %" PRIu16 "\n", ntohs(server_address.sin_port));

    for (;;) {
        struct sockaddr_in client_address;
        // printf("new\n");
        int client_fd = accept(socket_fd, (struct sockaddr *) &client_address,
                               &((socklen_t){sizeof(client_address)}));
        if (client_fd < 0) {
            syserr("accept");
        }

        // char const *client_ip = inet_ntoa(client_address.sin_addr);
        // uint16_t client_port = ntohs(client_address.sin_port);
        // // printf("accepted connection from %s:%" PRIu16 "\n", client_ip, client_port);

        // Set timeouts for the client socket.
        struct timeval to = {.tv_sec = MAX_WAIT, .tv_usec = 0};
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof to);
        setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof to);
        // sleep(5);
        CONN conn;
        if(read_CONN_TCP(client_fd,&conn) == -1) {
            send_RJT_TCP(-1, conn.session_id, client_fd);
            close(client_fd);
        }
        else if(conn.protocol_id != TCP_id) {
                error("wrong protocol send in CONN");
                send_RJT_TCP(-1, conn.session_id, client_fd);
                close(client_fd);
        }
        else {
            
            // printf("conn package type: %d\n", conn.packet_type);
            CONACC conacc = create_CONACC(conn.session_id);
            if(send_CONACC_TCP(client_fd,&conacc) == -1) {
                send_RJT_TCP(-1, conn.session_id, client_fd);
                close(client_fd);
            }
            else {
                ssize_t read_length = 0;
                uint64_t already_read = 0;
                DATA data;
                uint64_t packet_count = 0;
                uint64_t remaining_size = conn.byte_sequence_length;
                
                while(remaining_size > 0) {
                    read_length = read_DATA_TCP(client_fd,&data,&conn,&packet_count);
                    if(read_length == -1) {
                        // printf("sending RJT\n");
                        send_RJT_TCP(packet_count,conn.session_id,client_fd);
                        break;
                    } 
                    already_read += read_length;
                    remaining_size -= read_length;
                    packet_count++;
                }
                if(read_length != -1) send_RCVD_TCP(client_fd,conn.session_id);
                close(client_fd);
            }         
        }           
    }
    

    
    close(socket_fd);
}