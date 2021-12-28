#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>


#include "../../inc/common.h"

int client_queue_id;
int *server_keys;
// Zwraca id kolejki serwera, -1 jeśli nie ma miejsca
int connect_to_server(int server_nr, int client_queue_id, char nick[100])
{
    int server_id = msgget(server_nr, 0644 | IPC_CREAT);

    struct Mess registration_msg;
    clear_mess(&registration_msg);

    registration_msg.msgid = 2;
    registration_msg.from_client = client_queue_id;

    strcpy(registration_msg.from_client_name, nick);
    msgsnd(server_id, &registration_msg, sizeof(registration_msg) - sizeof(long), 0);

    struct Mess registration_ans;
    clear_mess(&registration_ans);

    msgrcv(client_queue_id, &registration_ans, sizeof(registration_msg) - sizeof(long), 2, 0);
    if (registration_ans.body[0] == '0')
    {
        return registration_ans.from_server;
    }
    else
    {
        return -1;
    }
}
// nastepuje tu pobranie kluczy kolejek, zostalo to rozbite na dwa bo potrzebujemy listy pozostalych serwerow a nie mozna przeslac jako parametr nieokreslonego pointera
void get_server_keys(char *config_path){
    int nr_of_lines=1;
    num_of_config_lines(&nr_of_lines,config_path);
    server_keys= malloc(sizeof(int) * nr_of_lines + 1);
    read_config(server_keys, config_path);
}

void end_of_work()
{
    msgctl(client_queue_id, IPC_RMID, NULL);
    printf("Klient zostal zamkniety\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, end_of_work);
    client_queue_id = msgget(IPC_PRIVATE, 0644 | IPC_CREAT);
    char nick[100] = "";
    printf("Podaj nazwę użytkownika: ");
    scanf("%s", &nick);

    get_server_keys("config.in");

   
    int server_queue_id = connect_to_server(server_keys[1], client_queue_id, nick);
    printf("ID serwera = %i\n", server_queue_id);

    return 0;
}