#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include "stdbool.h"


#include "err.h"
#include "common.h"
#include "sum-common.h"
#include "protconst.h"





int min(int x, int y) {
    if(x < y) return x;
    else return y;
}

int check_read_TCP(ssize_t expected_read_length, ssize_t read_length, uint64_t expected_Sid,
uint64_t Sid, uint64_t packet_number, uint64_t *expected_packet_number,
uint8_t packet_type, uint8_t expected_packet_type) {
    if (read_length < 0) {
        if (errno == EAGAIN) {
            error("timeout");
        }
        else {
            error("readn");
        }
        return -1;
    }
    else if (read_length == 0) {
        error("connection closed");
        return -1;
    }
    else if(check_packet_correctness(expected_Sid,Sid,packet_number,expected_packet_number,
    packet_type, expected_packet_type, expected_read_length, read_length)) return 1;
    return -1;
}

int check_write(ssize_t written_length, ssize_t expected_size) {
    if (written_length < 0) {
        error("send_CONACC written_length < 0 ");
        return -1;
    }
    else if ((size_t) written_length != (size_t) expected_size) {
        error("incomplete writen");
        return -1;
    }
    return 1;
}

bool check_ignorable_UDP(uint64_t expected_Sid,
uint64_t Sid, ssize_t ignorable_packet_type, ssize_t packet_type,
uint64_t packet_number, uint64_t *expected_packet_number) {
    return expected_Sid == Sid && ignorable_packet_type == packet_type 
    && (expected_packet_number == NULL || packet_number <= *expected_packet_number);
}

bool check_another_client_UDP(uint64_t expected_Sid,
uint64_t Sid,ssize_t packet_type) {
    return expected_Sid != Sid && packet_type == CONN_id;
}
 
int check_read_UDP(ssize_t expected_read_length, ssize_t read_length, uint64_t expected_Sid,
uint64_t Sid, uint64_t packet_number, uint64_t *expected_packet_number,
uint8_t packet_type, uint8_t expected_packet_type,ssize_t ignorable_packet_type) {
    if (read_length < 0) {
        if (errno == EAGAIN) {
            error("timeout");
            return TIMEOUT;
        }
        else {
            error("readn failed");
            return ERROR;
        }
    }
    else if(check_packet_correctness(expected_Sid,Sid,packet_number,expected_packet_number,
    packet_type, expected_packet_type, expected_read_length, read_length)) {
        return SUCCESS;
    }
    else if(check_ignorable_UDP(expected_Sid,Sid, ignorable_packet_type,
        packet_type,packet_number, expected_packet_number)) return IGNORABLE; 
    return ERROR;

}

uint16_t read_port(char const *string) {
    char *endptr;
    errno = 0;
    unsigned long port = strtoul(string, &endptr, 10);
    if (errno != 0 || *endptr != 0 || port > UINT16_MAX) {
        fatal("%s is not a valid port number", string);
    }
    return (uint16_t) port;
}

size_t read_size(char const *string) {
    char *endptr;
    errno = 0;
    unsigned long long number = strtoull(string, &endptr, 10);
    if (errno != 0 || *endptr != 0 || number > SIZE_MAX) {
        fatal("%s is not a valid number", string);
    }
    return number;
}

struct sockaddr_in get_server_address(char const *host, uint16_t port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *address_result;
    int errcode = getaddrinfo(host, NULL, &hints, &address_result);
    if (errcode != 0) {
        fatal("getaddrinfo: %s", gai_strerror(errcode));
    }

    struct sockaddr_in send_address;
    send_address.sin_family = AF_INET;   // IPv4
    send_address.sin_addr.s_addr =       // IP address
            ((struct sockaddr_in *) (address_result->ai_addr))->sin_addr.s_addr;
    send_address.sin_port = htons(port); // port from the command line

    freeaddrinfo(address_result);

    return send_address;
}


CONN create_CONN(enum ProtocolIdentifier protocol_id, uint64_t size){
    CONN conn;
    conn.packet_type = CONN_id;
    srand(time(NULL));
    conn.session_id = rand();
    conn.protocol_id = protocol_id;
    conn.byte_sequence_length = size;
    return conn;   
}

CONACC create_CONACC(uint64_t session_id) {
    CONACC conacc;
    conacc.packet_type = CONACC_id;
    conacc.session_id = session_id;
    return conacc;
}

int send_CONACC_TCP(int socket_fd, CONACC *data_to_send) {
    ssize_t written_length = writen(socket_fd, data_to_send, sizeof (*data_to_send));
    return check_write(written_length,sizeof(*data_to_send));    
}


int read_CONACC_TCP(int client_fd, CONACC *data, uint64_t expected_Sid) {
    ssize_t read_length = readn(client_fd, data, sizeof (*data));
    return check_read_TCP(sizeof(*data),read_length,expected_Sid,data->session_id,0,NULL,
    data->packet_type,CONACC_id);
}

int get_identifier(char const *id) {
    if(strcmp(id,"tcp") == 0) return TCP_id;
    else if(strcmp(id,"udp") == 0) return UDP_id;
    else if(strcmp(id,"udpr") == 0) return UDP_WITH_RETRANSMISSION_id;
    else return ERROR_ID_protocol;
}


char *read_input(int *size) {
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    char *buffer = (char *)malloc(buffer_size * sizeof(char));
    if (buffer == NULL) {
        syserr("Memory allocation failed");
        exit(1);
    }

    size_t total_length = 0;
    int c;
    while ((c = fgetc(stdin)) != EOF) {
        // Jeśli bufor jest zapełniony, zwiększ jego rozmiar
        if (total_length >= buffer_size - 1) {
            buffer_size *= GROWTH_FACTOR;
            char *temp = (char *)realloc(buffer, buffer_size);
            if (temp == NULL) {
                free(buffer);
                syserr("Memory reallocation failed");
                exit(1);
            }
            buffer = temp;
        }

        // Przechowaj wczytany znak w buforze
        buffer[total_length++] = (char)c;
    }

    // Zakończ bufor znakiem null
    // buffer[total_length] = '\0';
    *size = total_length;
    return buffer;
}


int send_CONN_TCP(int socket_fd, CONN *data_to_send) {
    ssize_t written_length = writen(socket_fd, data_to_send, sizeof (*data_to_send));
    return check_write(written_length,sizeof(*data_to_send)); 
}

int read_CONN_TCP(int client_fd, CONN *data) {
    ssize_t read_length = readn(client_fd, data, sizeof (*data));
    return check_read_TCP(sizeof(*data),read_length,data->session_id,data->session_id,0,NULL,
    data->packet_type,CONN_id);
}

int send_RCVD_TCP(int socket_fd, int session_id) {
    RCVD data_to_send;
    data_to_send.packet_type = RCVD_id;
    data_to_send.session_id = session_id;
    ssize_t written_length = writen(socket_fd, &data_to_send, sizeof data_to_send);
    return check_write(written_length,sizeof(data_to_send)); 
}

int read_RCVD_TCP(int client_fd, RCVD *data, uint64_t expected_Sid) {
    ssize_t read_length = readn(client_fd, data, sizeof (*data));
    return check_read_TCP(sizeof(*data),read_length,expected_Sid,data->session_id,
    0,NULL,data->packet_type,RCVD_id); 
}
   
bool check_packet_correctness(uint64_t expected_session_id, uint64_t session_id, uint64_t packet_number,
 uint64_t *expected_packet_number, uint8_t packet_type, uint8_t expected_packet_type,
 ssize_t expected_read_length, ssize_t read_length) {
    if(expected_packet_type != packet_type) {
        error("expected packet type = %d but received : %d", expected_packet_type, packet_type);
        return false;
    }
    if(expected_session_id != session_id) {
        error("expected_session_id = %ld but received: %ld", expected_session_id, session_id);
        return false;
    }
    if(expected_packet_number != NULL && *expected_packet_number != packet_number) {
        error("expected_packet_number = %ld but received: %ld", *expected_packet_number, packet_number);
        return false;
    }
    if(expected_read_length != read_length) {
        error("expected read_size = %ld but received: %ld", expected_read_length, read_length);
        return false;
    }
    return true;
}

int send_RJT_TCP(uint64_t packet_number, uint64_t session_id, int socket_fd) {
    RJT rjt;
    rjt.packet_type = RJT_id;
    rjt.packet_number = packet_number;
    rjt.session_id = session_id;
    ssize_t written_length = writen(socket_fd, &rjt, sizeof rjt);
    return check_write(written_length,sizeof(rjt)); 
}

ssize_t send_DATA_TCP(int socket_fd, DATA *data_to_send) {
    ssize_t written_length;
    int result;
    char combined_data[sizeof(*data_to_send) + data_to_send->data_byte_count];

    memcpy(combined_data, data_to_send, sizeof(*data_to_send));

    memcpy(combined_data + sizeof(*data_to_send), data_to_send->data, data_to_send->data_byte_count);

    // Send the combined data
    written_length = writen(socket_fd, combined_data, sizeof(combined_data));
    result = check_write(written_length, sizeof(combined_data));
    if(result == -1)
        return result;

    return data_to_send->data_byte_count;
}

ssize_t read_DATA_TCP(int client_fd, DATA *data, CONN *conn, uint64_t *expected_packet_number) {
    ssize_t read_length = readn(client_fd, data, sizeof (*data));
    int result = check_read_TCP(sizeof(*data),read_length,conn->session_id,data->session_id,
    data->packet_number,expected_packet_number,data->packet_type,DATA_id);
    if(result == -1 || data->data_byte_count > MAX_DATA_SIZE) return result;
    
    int to_be_read_size = data->data_byte_count;
    char input[to_be_read_size];
    
    read_length = readn(client_fd, input, to_be_read_size);
    result = check_read_TCP(to_be_read_size,read_length,0,0,*expected_packet_number,expected_packet_number,
    0,0);
    if(result == -1) return result;

    write(STDOUT_FILENO, input, to_be_read_size);
    fflush(stdout);
    return data->data_byte_count;
}




int send_CONN_UDP(struct sockaddr_in server_address, int socket_fd, CONN *conn) {
    int send_flags = 0;
    socklen_t address_length = (socklen_t) sizeof(server_address);
    ssize_t sent_length = sendto(socket_fd, conn, sizeof(*conn), send_flags,
                                    (struct sockaddr *) &server_address, address_length);
    return check_write(sent_length,sizeof(*conn));
}

int read_CONN_UDP(int socket_fd, CONN *conn, struct sockaddr_in *client_address) {
    int flags = 0;
    socklen_t address_length = (socklen_t) sizeof(*client_address);

    ssize_t received_length = recvfrom(socket_fd, conn, sizeof(*conn), flags,
                                (struct sockaddr *) client_address, &address_length);
    
    int result = check_read_UDP(sizeof(*conn),received_length,0,0,0,NULL,conn->packet_type,CONN_id,-1);
    if(result != SUCCESS) return result;
    else if(conn->protocol_id != UDP_id && conn->protocol_id != UDP_WITH_RETRANSMISSION_id) {
        error("wrong protocol id");
        return ERROR;
    }
    else return SUCCESS;
    
}

int send_CONACC_UDP(int socket_fd, CONACC *conacc,struct sockaddr_in *client_address) {
    int send_flags = 0;
    socklen_t address_length = (socklen_t) sizeof(*client_address);
    ssize_t sent_length = sendto(socket_fd, conacc, sizeof(*conacc), send_flags,
                                    (struct sockaddr *) client_address, address_length);
    return check_write(sent_length,sizeof(*conacc));
}

int read_CONACC_UDP(int socket_fd, CONACC *conacc, int retransmission_count,
struct sockaddr_in server_address, CONN *conn) {
    int receive_flags = 0;
    ssize_t received_length = recvfrom(socket_fd, conacc, sizeof(*conacc), receive_flags,
                                           NULL, NULL);
    int result = check_read_UDP(sizeof(*conacc),received_length,conn->session_id,conacc->session_id,
    0,NULL,conacc->packet_type,CONACC_id,-1);
    if(result == TIMEOUT) {
        if(retransmission_count > 0) {
            if(send_CONN_UDP(server_address,socket_fd,conn) != SUCCESS) return ERROR;
            return read_CONACC_UDP(socket_fd,conacc,retransmission_count--,server_address,conn);
        }
        else return ERROR;
    } 
    return result;

}

ssize_t send_DATA_UDP(int socket_fd, DATA *data ,struct sockaddr_in *client_address) {
    int send_flags = 0;
    socklen_t address_length = sizeof(*client_address);
    ssize_t sent_length;
    int result;

    size_t combined_data_size = sizeof(*data) + data->data_byte_count;

    char combined_data[combined_data_size];


    // Copy the first part of the data
    memcpy(combined_data, data, sizeof(*data));

    // Copy the second part of the data after the first part
    memcpy(combined_data + sizeof(*data), data->data, data->data_byte_count);

    // Send the combined data
    sent_length = sendto(socket_fd, combined_data, combined_data_size, send_flags,
                        (struct sockaddr *) client_address, address_length);

    // Check for write errors
    result = check_write(sent_length, combined_data_size);
    if (result != SUCCESS) return ERROR;
    return data->data_byte_count;
}

bool compareAddresses(const struct sockaddr_in *client_address, const struct sockaddr_in *received_from) {
    if (client_address->sin_family != received_from->sin_family) {
        return false;  
    }
    if (memcmp(&client_address->sin_addr, &received_from->sin_addr, sizeof(struct in_addr)) != 0) {
        return false; 
    }
    if (client_address->sin_port != received_from->sin_port) {
        return false;  
    }
    return true;
}


ssize_t read_DATA_UDP(int socket_fd, DATA *data, struct sockaddr_in *client_address,
 CONN *conn, uint64_t *expected_packet_number, ssize_t ignorable_packet_type, int retransmisions_count) {
    int flags = 0;
    
    ssize_t received_length;
    char combined_data[sizeof(*data) + MAX_DATA_SIZE];

    struct sockaddr_in received_from;
    socklen_t address_length = sizeof(received_from);

    received_length = recvfrom(socket_fd, combined_data, sizeof(combined_data), flags,
                            (struct sockaddr *) &received_from, &address_length);
    

    if(!compareAddresses(client_address,&received_from)) {
        send_CONRJT_UDP(socket_fd,conn->session_id,&received_from);
        return read_DATA_UDP(socket_fd,data,client_address,conn,expected_packet_number,ignorable_packet_type,retransmisions_count);
    }

    if(received_length < (ssize_t) sizeof(*conn)) {
        if (errno == EAGAIN) {
            error("timeout");
            if(retransmisions_count > 0) {
                if(expected_packet_number != NULL && *expected_packet_number == 0) {
                    CONACC conacc = create_CONACC(conn->session_id);
                    if(send_CONACC_UDP(socket_fd, &conacc, client_address) != 1) return -1;
                } 
                else send_ACC_UDP(socket_fd,conn->session_id,client_address, (*expected_packet_number)-1);
                return read_DATA_UDP(socket_fd,data,client_address,conn,expected_packet_number,ignorable_packet_type,retransmisions_count-1);
            }
        }
        error("ERROR timeout");        
        return ERROR;
    } 

    memcpy(data, combined_data, sizeof(*data));

    
    int result = check_read_UDP(sizeof(*data) + data->data_byte_count, received_length, conn->session_id,
    data->session_id,data->packet_number,expected_packet_number,data->packet_type,DATA_id,ignorable_packet_type);

    if(data->data_byte_count > MAX_DATA_SIZE) return -1;
    
    if(result == TIMEOUT) {
        if(retransmisions_count > 0) {
            if(expected_packet_number != NULL && *expected_packet_number == 0) {
                CONACC conacc = create_CONACC(conn->session_id);
                send_CONACC_UDP(socket_fd, &conacc, client_address);
            } 
            else send_ACC_UDP(socket_fd,conn->session_id,client_address, (*expected_packet_number)-1);
            return read_DATA_UDP(socket_fd,data,client_address,conn,expected_packet_number,ignorable_packet_type,retransmisions_count-1);
        }
        return ERROR;
    }
    else if(result == IGNORABLE) {
        return read_DATA_UDP(socket_fd,data,client_address,conn,expected_packet_number,
            ignorable_packet_type,retransmisions_count);
    } 
    else if(result == ERROR) return ERROR;

    char input[data->data_byte_count];

    // Extract the second part of the data after the first part
    memcpy(input, combined_data + sizeof(*data), data->data_byte_count);

    write(STDOUT_FILENO, input, data->data_byte_count);
    fflush(stdout);
    return data->data_byte_count;

}

int send_RCVD_UDP(int socket_fd, uint64_t session_id,struct sockaddr_in *client_address) {
    RCVD rcvd;
    rcvd.packet_type = RCVD_id;
    rcvd.session_id = session_id;
    int send_flags = 0;
    socklen_t address_length = (socklen_t) sizeof(*client_address);
    ssize_t sent_length = sendto(socket_fd, &rcvd, sizeof(rcvd), send_flags,
                                    (struct sockaddr *) client_address, address_length);
    return check_write(sent_length,sizeof(rcvd));
}

int read_RCVD_UDP(int client_fd, RCVD *data, uint64_t expected_Sid) {
    ssize_t read_length = readn(client_fd, data, sizeof (*data));
    return check_read_UDP(sizeof(*data),read_length,expected_Sid,data->session_id,
    0,NULL,data->packet_type,RCVD_id,-1);
}

int send_RJT_UDP(int socket_fd, uint64_t session_id, struct sockaddr_in *client_address, uint64_t packet_number) {
    RJT rjt;
    rjt.packet_type = RJT_id;
    rjt.packet_number = packet_number;
    rjt.session_id = session_id;
    int send_flags = 0;
    socklen_t address_length = (socklen_t) sizeof(*client_address);
    ssize_t sent_length = sendto(socket_fd, &rjt, sizeof(rjt), send_flags,
                                    (struct sockaddr *) client_address, address_length);
    return check_write(sent_length,sizeof(rjt));
}

int send_CONRJT_UDP(int socket_fd, uint64_t session_id, struct sockaddr_in *client_address) {
    CONRJT conrjt;
    conrjt.packet_type = CONRJT_id;
    conrjt.session_id = session_id;
    int send_flags = 0;
    socklen_t address_length = (socklen_t) sizeof(*client_address);
    ssize_t sent_length = sendto(socket_fd, &conrjt, sizeof(conrjt), send_flags,
                                    (struct sockaddr *) client_address, address_length);
    return check_write(sent_length,sizeof(conrjt));
}

int set_timeout(int socket_fd, int timeout_value) {

    struct timeval timeout;
    timeout.tv_sec = timeout_value;
    timeout.tv_usec = 0;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
        return -1;
    }
    return 1;
}

int send_ACC_UDP(int socket_fd, uint64_t session_id, struct sockaddr_in *client_address, uint64_t packet_number) {
    ACC acc;
    acc.packet_number = packet_number;
    acc.packet_type = ACC_id;
    acc.session_id = session_id;
    int send_flags = 0;
    socklen_t address_length = (socklen_t) sizeof(*client_address);
    ssize_t sent_length = sendto(socket_fd, &acc, sizeof(acc), send_flags,
                                    (struct sockaddr *) client_address, address_length);
    return check_write(sent_length,sizeof(acc));
}

int read_ACC_UDP(int client_fd, ACC *acc, uint64_t expected_Sid,uint64_t *expected_packet_number,
 ssize_t ignorable_packet_type, int retransmisions_count, DATA *data_resend, struct sockaddr_in *server_address) {
    ssize_t read_length = readn(client_fd, acc, sizeof (*acc));
    int result;
    if(*expected_packet_number == 0) {
        result = check_read_UDP(sizeof(*acc),read_length,expected_Sid,acc->session_id,acc->packet_number,
    NULL,acc->packet_type,ACC_id, ignorable_packet_type);
    }
    else {
        result = check_read_UDP(sizeof(*acc),read_length,expected_Sid,acc->session_id,acc->packet_number,
    expected_packet_number,acc->packet_type,ACC_id, ignorable_packet_type);
    }

    if(result == TIMEOUT) {
        if(retransmisions_count > 0) {
            if(send_DATA_UDP(client_fd,data_resend,server_address) != (ssize_t) data_resend->data_byte_count) return ERROR;
            return read_ACC_UDP(client_fd,acc,expected_Sid,expected_packet_number,ignorable_packet_type,retransmisions_count-1,
            data_resend, server_address);
        }
        return ERROR;
    }
    else if(result == IGNORABLE) return read_ACC_UDP(client_fd,acc,expected_Sid,expected_packet_number,ignorable_packet_type,retransmisions_count,
            data_resend, server_address);
    else if(result == ERROR) {
        return ERROR;
    } 
    return SUCCESS;
}