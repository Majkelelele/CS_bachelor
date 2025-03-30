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





void run_UDP_client(char const *host, uint16_t port, enum ProtocolIdentifier protocol_id) {
    
    struct sockaddr_in server_address = get_server_address(host, port);
    // char const *server_ip = inet_ntoa(server_address.sin_addr);
    // uint16_t server_port = ntohs(server_address.sin_port);

    int size = 0;
    char *input = read_input(&size);

    // Create a socket.
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    if(set_timeout(socket_fd, MAX_WAIT) == -1) return;

    
    // size_t message_length = size;    
    CONN conn = create_CONN(protocol_id,size);
    if(send_CONN_UDP(server_address,socket_fd,&conn) != -1) {
        // printf("UDP: sent CONN: byte_length: %ld\n packet Type: %d\n, protocol_id %d\n session_id: %ld\n", conn.byte_sequence_length, conn.packet_type,
        // conn.protocol_id, conn.session_id);
        CONACC conacc;
        int retransmissions = 0;
        if(protocol_id == UDP_WITH_RETRANSMISSION_id)  retransmissions = MAX_RETRANSMITS;
        if(read_CONACC_UDP(socket_fd,&conacc,retransmissions,server_address,&conn) != -1) {
            // printf("received CONACC packet_type: %d\n session_id %ld\n", conacc.packet_type, conacc.session_id);
            // sleep(6);
            DATA data_to_send;
            ssize_t sent = 0;
            for(int i = 0; size > 0; i++) {
                int packet_data_size = min(size,MAX_PACKET_SIZE);

                data_to_send.data = input + i*sizeof(char)*MAX_PACKET_SIZE;
                data_to_send.data_byte_count = packet_data_size*sizeof(char);
                data_to_send.packet_number = i;
                data_to_send.packet_type = DATA_id;
                data_to_send.session_id = conn.session_id;
                sent = send_DATA_UDP(socket_fd,&data_to_send,&server_address);
                // printf("sending %d bytes\n",data_to_send.data_byte_count);
                if(sent == -1) break;
                size -= sent;
                ACC acc;
                uint64_t packet_number = (uint64_t) i;
                int ignorable_packet_type = ACC_id;
                if(i == 0) ignorable_packet_type = CONACC_id;
                if(protocol_id == UDP_WITH_RETRANSMISSION_id && 
                !(read_ACC_UDP(socket_fd,&acc,conn.session_id,&packet_number,ignorable_packet_type,
                MAX_RETRANSMITS,&data_to_send,&server_address) == 1)) {
                    sent = -1;
                    break;
                }
                // if(i == 2) sleep(5);
            }
            RCVD received;
            if(sent != -1 && read_RCVD_UDP(socket_fd,&received,conn.session_id) != -1) {
                if(received.packet_type != RCVD_id || 
                received.session_id != conn.session_id) fprintf(stderr,"ERROR: received wrong RCVD \n");
                // printf("received RCVD packet_type: %d\n session_id %ld\n", received.packet_type, received.session_id);
            }               
        }
    }
    free(input);
    close(socket_fd);
}
