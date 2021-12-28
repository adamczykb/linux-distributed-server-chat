#include <time.h>
#include <string.h>
#include <fcntl.h>
#ifndef PSIW_COMMON_H
#define PSIW_COMMON_H

struct Mess
{
    long msgid;
    int from_server;
    int from_client;
    char from_client_name[100];
    int to_chanel;
    char to_client_name[100];
    time_t timestamp;
    char body[1024];
};

void clear_mess(struct Mess *mess);
void read_config(int *servers_keys, char *path_to_config);
void num_of_config_lines(int *nr_of_lines, char *path_to_config);
#endif