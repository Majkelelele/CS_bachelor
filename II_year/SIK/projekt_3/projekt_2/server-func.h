#ifndef ERR_H
#define ERR_H

#include <string>
#include <vector>
#include "common.h"
#include <map>
#include <pthread.h>  
#include <sys/socket.h>

extern pthread_mutex_t global_mutex;

extern int main_descriptor;

extern pthread_mutex_t mutexes[CLIENTS];
extern pthread_barrier_t final_barrier;
extern std::map<std::string, int> position_id_map;
extern pthread_barrier_t clients_barrier;
extern std::vector<Deal> deals;

extern bool finish;
extern std::string current_cards_list;
extern int current_deal;
extern int players_played_in_round;
extern std::string who_won;


/* Print usage information. */
void printUsage();

/* Parse command-line arguments. */
void parseArguments(int argc, char* argv[], int* port, std::string* file, int* timeout);





std::vector<std::string> splitCards(const std::string& line);
void parseDealsFromFile(const std::string& filename);
void printDeals(const std::vector<Deal>& deals);
void initilizeMap();
void send_deal_to_client(int client_fd, const std::string &deal_type,
  std::string starting_client,const std::vector<std::string> &cards,const std::string &ip_sender,
 uint16_t port_sender, const std::string &ip_local, uint16_t port_local);
std::string find_who_next(std:: string current);
void *handle_connection(void *client_fd_ptr);
int start_server(int socket_fd);
void destroy_and_finish();
int accept_client(int client_id, struct sockaddr_in *client_address, socklen_t *client_address_len);
std::string process_IAM_message(int client_fd, const std::string &ip_sender,
 uint16_t port_sender, const std::string &ip_local, uint16_t port_local);
void trick_communication(int client_id, std::string position, int client_fd, const std::string &ip_sender,
 uint16_t port_sender, const std::string &ip_local, uint16_t port_local, int current_round);
void send_score_to_client(int client_fd, const std::string &ip_sender,
 uint16_t port_sender, const std::string &ip_local, uint16_t port_local);
int send_taken(int socket_fd, std::string card_list, int numer_lewy, std::string client_position,
const std::string &ip_sender, uint16_t port_sender, const std::string &ip_local, uint16_t port_local);
void send_total_to_client(int client_fd, const std::string &ip_sender,
 uint16_t port_sender, const std::string &ip_local, uint16_t port_local);
std::string summarize_trick(int current_round, int trick_number);


#endif
