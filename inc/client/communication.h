
#ifndef PSIW_COMMUNICATION_H
#define PSIW_COMMUNICATION_H

void connect_to_server(int server_nr, char nick[100], int client_queue_id, int *result);
void heartbeat(int client_queue_id,  char* nick, int server_queue_id);

#endif