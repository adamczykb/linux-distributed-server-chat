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
#define MAX_CHANNEL 50

struct Mess
{
    long msgid;
    int from_server;
    int from_client;
    char from_client_name[100];
    int to_chanel;
    int to_user;
    int broadcasted;
    char to_client_name[100];
    time_t timestamp;
    char body[1024];
    int for_server;
};

struct User
{
    int queue_id;
    char nick[100];
    struct Mess messages[100];
    int server_host;
    int free; // 0 -> nie 1 -> tak
};

struct Channel
{
    int id;
    char name[100];
    struct User users[10];
    struct Mess messages[100]; //wiem ze mialo byc 10 ale mysle ze 10 ma przysylac serwer
    int free;                  // 0 -> nie 1 -> tak
    int usr_signed;            // 0 -> nie 1 -> tak
};

void clear_mess(struct Mess *mess);
void read_config(int *servers_keys, char *path_to_config);
void num_of_config_lines(int *nr_of_lines, char *path_to_config);
void boxDescription(WINDOW *pwin, const char *title); /* pwin (WINDOW*) -> okno ncurses, title (const char *) -> treść, która ma zostać wyświetlona nad oknem */
void add_user_to_channel(struct Channel *channels, struct Mess *mess, int *result);
void remove_user_from_channel(struct Channel *channels, struct Mess *mess,int *result);
void add_msg_to_channel(struct Channel *channels, struct Mess *mess);

#endif