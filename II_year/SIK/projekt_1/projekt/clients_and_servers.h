#ifndef CLIENTS_AND_SERVERS_H
#define CLIENTS_AND_SERVERS_H

#include <stdint.h>

void run_TCP_client(char const *host, uint16_t port);
void run_TCP_server(uint16_t port);

#endif 

