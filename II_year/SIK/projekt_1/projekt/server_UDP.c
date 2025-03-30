#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "sum-common.h"
#include "err.h"
#include "common.h"
#include "protconst.h"




void run_UDP_server(uint16_t port) {
    // Create a socket. Buffer should not be allocated on the stack.
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }
    enum ProtocolIdentifier protocol_id;
    // Bind the socket to a concrete address.
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // Listening on all interfaces.
    server_address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        syserr("bind");
    }

    // printf("listening on port %" PRIu16 "\n", port);
    struct sockaddr_in client_address;

    for(;;) {
        CONN conn;
        // printf("new\n");
        if(read_CONN_UDP(socket_fd,&conn,&client_address) != -1) {
            
            protocol_id = conn.protocol_id;
            if(set_timeout(socket_fd, MAX_WAIT) == -1) return;

            CONACC conacc = create_CONACC(conn.session_id);
            int retransmissions = 0;
            if(protocol_id == UDP_WITH_RETRANSMISSION_id)  retransmissions = MAX_RETRANSMITS;
            if(send_CONACC_UDP(socket_fd,&conacc,&client_address) != -1) {
                ssize_t read_length = 0;
                uint64_t already_read = 0;
                DATA data;
                uint64_t packet_count = 0;
                uint64_t remaining_size = conn.byte_sequence_length;
                
                while(remaining_size > 0) {
                    if(packet_count == 0) {
                         read_length = read_DATA_UDP(socket_fd,&data,&client_address
                    ,&conn,&packet_count,CONN_id,retransmissions);
                        // printf("here1\n");
                    }
                    else {
                         read_length = read_DATA_UDP(socket_fd,&data,&client_address,
                    &conn,&packet_count,DATA_id,retransmissions);
                        // printf("here2 result: %ld\n", read_length);
                    }

                    if(read_length == -1) break;
                    already_read += read_length;
                    remaining_size -= read_length;
                    if(protocol_id == UDP_WITH_RETRANSMISSION_id && 
                    !(send_ACC_UDP(socket_fd,conn.session_id,&client_address,packet_count) == 1)) {
                        read_length = -1;
                        break;
                    } 

                    packet_count++;
                }
                if(read_length != -1) send_RCVD_UDP(socket_fd,conn.session_id,&client_address);
                else {
                    // error("sending RJT read_length == -1");
                    send_RJT_UDP(socket_fd,conn.session_id,&client_address,0);
                } 
                if(set_timeout(socket_fd, 0) == -1) return;
                
                // printf("exchange finished\n");
            }
            else {
                // error("sending RJT 2");
                send_RJT_UDP(socket_fd,conn.session_id,&client_address,0);
            }
        }
        else {
            // error("sending RJT 3");
            send_RJT_UDP(socket_fd,conn.session_id,&client_address,0);
        }
    }
    
    

    close(socket_fd);
}
