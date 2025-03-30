#ifndef MIM_COMMON_H
#define MIM_COMMON_H

#include "stdbool.h"

#define INITIAL_BUFFER_SIZE 64
#define GROWTH_FACTOR 2
#define MAX_PACKET_SIZE 10000
#define MAX_DATA_SIZE 64000

// Definicja identyfikatora protokołu
enum ProtocolIdentifier {
    ERROR_ID_protocol = 0,
    TCP_id = 1,
    UDP_id = 2,
    UDP_WITH_RETRANSMISSION_id = 3
};

enum PacketIdentifier {
    ERROR_id = 0,
    CONN_id = 1,
    CONACC_id = 2,
    CONRJT_id = 3,
    DATA_id = 4,
    ACC_id = 5,
    RJT_id = 6,
    RCVD_id = 7
};


enum UDP_read_error {
    ERROR = -1,
    TIMEOUT = 0,
    SUCCESS = 1,
    IGNORABLE = 2,
    ANOTHER_CLIENT_CONN = 3
};

// Struktura reprezentująca pakiet nawiązania połączenia (K->S)
typedef struct __attribute__((__packed__)){
    uint8_t packet_type;             // Identyfikator typu pakietu (8 bitów)
    uint64_t session_id;             // Losowy identyfikator sesji (64 bity)
    uint8_t protocol_id;  // Identyfikator protokołu (8 bitów)
    uint64_t byte_sequence_length;   // Długość ciągu bajtów (64 bity)
} CONN;


typedef struct  __attribute__((__packed__)){
    uint8_t packet_type;   // Identyfikator typu pakietu (8 bitów)
    uint64_t session_id;   // Identyfikator sesji (64 bity)
} CONACC;


typedef struct __attribute__((__packed__)) {
    uint8_t packet_type;   // Identyfikator typu pakietu (8 bitów)
    uint64_t session_id;   // Identyfikator sesji (64 bity)
} CONRJT;


// Struktura reprezentująca pakiet danych (K->S)
typedef struct __attribute__((__packed__)) {
    uint8_t packet_type;               // Identyfikator typu pakietu (8 bitów)
    uint64_t session_id;               // Identyfikator sesji (64 bity)
    uint64_t packet_number;            // Numer pakietu (64 bity)
    uint32_t data_byte_count;          // Liczba bajtów danych w pakiecie (32 bity)
    char *data;                     // Dane (zmiennej długości)
} DATA;


// Struktura reprezentująca potwierdzenie pakietu z danymi (S->K)
typedef struct __attribute__((__packed__)) {
    uint8_t packet_type;        // Identyfikator typu pakietu (8 bitów)
    uint64_t session_id;        // Identyfikator sesji (64 bity)
    uint64_t packet_number;     // Numer pakietu (64 bity)
} ACC;

typedef struct __attribute__((__packed__)){
    uint8_t packet_type;        // Identyfikator typu pakietu (8 bitów)
    uint64_t session_id;        // Identyfikator sesji (64 bity)
    uint64_t packet_number;     // Numer pakietu (64 bity)
} RJT;

typedef struct  __attribute__((__packed__)){
    uint8_t packet_type;   // Identyfikator typu pakietu (8 bitów)
    uint64_t session_id;   // Identyfikator sesji (64 bity)
} RCVD;

uint16_t read_port(char const *string);
size_t   read_size(char const *string);
CONN create_CONN(enum ProtocolIdentifier id, uint64_t size);
int get_identifier(char const *id);
char *read_input();
int send_CONN_TCP(int socket_fd, CONN *data_to_send);
int read_CONN_TCP(int client_fd, CONN *data);
CONACC create_CONACC(uint64_t session_id);
int send_CONACC_TCP(int socket_fd, CONACC *data_to_send);
int read_CONACC_TCP(int client_fd, CONACC *data, uint64_t expected_Sid);
int min(int x, int y);
int send_RCVD_TCP(int socket_fd, int session_id);
int read_RCVD_TCP(int client_fd, RCVD *data, uint64_t expected_Sid);
bool check_packet_correctness(uint64_t expected_session_id, uint64_t session_id, uint64_t packet_number,
 uint64_t *expected_packet_number, uint8_t packet_type, uint8_t expected_packet_type,
 ssize_t expected_read_length, ssize_t read_length);
int send_RJT_TCP(uint64_t packet_number, uint64_t session_id, int socket_fd);
ssize_t send_DATA_TCP(int socket_fd, DATA *data_to_send);
ssize_t read_DATA_TCP(int client_fd, DATA *data, CONN *conn, uint64_t *expected_packet_number);
int send_CONN_UDP(struct sockaddr_in server_address, int socket_fd, CONN *conn);
int read_CONN_UDP(int socket_fd, CONN *conn,struct sockaddr_in *client_address);
int send_CONACC_UDP(int socket_fd, CONACC *conacc,struct sockaddr_in *client_address);
int read_CONACC_UDP(int socket_fd, CONACC *conacc, int retransmission_count,
struct sockaddr_in server_address, CONN *conn);
ssize_t read_DATA_UDP(int socket_fd, DATA *data, struct sockaddr_in *client_address,
 CONN *conn, uint64_t *expected_packet_number, ssize_t ignorable_read_length, int retransmisions_count);
ssize_t send_DATA_UDP(int socket_fd, DATA *data ,struct sockaddr_in *client_address);
int send_RCVD_UDP(int socket_fd, uint64_t session_id,struct sockaddr_in *client_address);
int read_RCVD_UDP(int client_fd, RCVD *data, uint64_t expected_Sid);
int send_RJT_UDP(int socket_fd, uint64_t session_id, struct sockaddr_in *client_address, uint64_t packet_number);
int set_timeout(int socket_fd, int timeout_value);
int send_ACC_UDP(int socket_fd, uint64_t session_id, struct sockaddr_in *client_address, uint64_t packet_number);
int read_ACC_UDP(int client_fd, ACC *acc, uint64_t expected_Sid,uint64_t *expected_packet_number,
 ssize_t ignorable_packet_type, int retransmisions_count, DATA *data_resend, struct sockaddr_in *server_address);
int send_CONRJT_UDP(int socket_fd, uint64_t session_id, struct sockaddr_in *client_address);

struct sockaddr_in get_server_address(char const *host, uint16_t port);

#endif