#include <time.h>
#include <string.h>

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
#endif