#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>

#include <signal.h>

#ifndef PSIW_COMMON_H
#define PSIW_COMMON_H
#define MAX_LOG 500

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
void boxDescription(WINDOW *pwin, const char *title); /* pwin (WINDOW*) -> okno ncurses, title (const char *) -> treść, która ma zostać wyświetlona nad oknem */

#endif