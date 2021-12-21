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

struct Mess
{
    long msgid;
    int from_server;
    int from_client;
    char from_client_name[100];
    int to_chanel;
    char to_client_name[100];
    long timestamp;
    char body[1024];
};

// Zwraca id kolejki serwera, -1 jeśli nie ma miejsca
int connect_to_server(int server_nr, int client_queue_id, char nick[100]){
    int server_id = msgget(server_nr, 0644|IPC_CREAT);
    struct Mess registration_msg;
    registration_msg.msgid = 2;
    registration_msg.from_client = client_queue_id;
    strcpy(registration_msg.from_client_name, nick);
    msgsnd(server_id, &registration_msg, sizeof(registration_msg)-sizeof(long), 0);
    struct Mess registration_ans;
    msgrcv(client_queue_id, &registration_ans, sizeof(registration_msg)-sizeof(long), 2, 0);
    if (registration_ans.body[0] == '0'){
        return registration_ans.from_server;
    }
    else{
        return -1;
    }
}


int main(int argc, char *argv[]){
    char nick[100] = "";
    printf("Podaj nazwę użytkownika: ");
    scanf("%s", &nick);
    int client_queue_id = msgget(55555, 0644|IPC_CREAT);
    int server_queue_id = connect_to_server(atoi(argv[1]), client_queue_id, nick);
    printf("ID serwera = %i\n", server_queue_id);

    return 0;
}