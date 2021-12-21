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

int nr_of_clients=0;
char current_clients[5][100];


// !!!Opisz to jakoś Bartek, bo nadal nie ogarniam co to robi!!!
void boxDescription(WINDOW *pwin, const char *title)
{
    int x, stringsize;
    stringsize = 4 + strlen(title);
    x = 1;
    mvwaddch(pwin, 0, x, ' ');
    waddstr(pwin, title);
    waddch(pwin, ' ');
}

// Zwraca wskaźnik na tablicę [ilość_serwerów, id_kolejki_serwera1, ...]
int* load_config(){
    // odczytanie pliku
    char* filename = "config.txt";
    int file = open(filename, O_RDWR);
    char buff[100] = "";
    int nr_of_lines = 1;
    for (int i=0; read(file, &buff[i], 1)>0; i++)
        if (buff[i-1]=='\n' && buff[i]>=48 && buff[i]<=57) nr_of_lines += 1;
    close(file);

    // zapisanie elementów z pliku do listy
    int *queue_ids = (int *)malloc(sizeof(int)*nr_of_lines+1);
    queue_ids[0] = nr_of_lines;
    int current_v=0, current_i=1;
    for (int i=0; i<100; i++){
        if (buff[i]=='\0') break;
        if (buff[i]=='\n'){
            queue_ids[current_i] = msgget(current_v, 0644|IPC_CREAT);
            current_v = 0;
            current_i += 1;
            continue;
        }
        current_v = current_v*10+buff[i]-48;
    }
    if (current_v != 0) queue_ids[current_i] = msgget(current_v, 0644|IPC_CREAT);
    return queue_ids;
}

int registration(char nickname[100]){
    // Sprawdzenie czy nazwa już nie występuje
    for (int i=0; i<5; i++){
        if (!strcmp(current_clients[i], nickname)){
            return -1;
        }
    }
    // Dodanie użytkownika
    strcpy(current_clients[nr_of_clients], nickname);
    nr_of_clients++;
    return 0;
}

int logout(char nickname[100]){
    int placement;
    // Sprawdzenie gdzie na liście występuje ta nazwa
    for (int i=0; i<nr_of_clients; i++){
        if (strcmp(current_clients[i], nickname)){
            placement = i;
            break;
        }
    }
    // Usunięcie użytkownika i przesunięcie pozostałych nazw
    for (int i=placement; i<nr_of_clients; i++){
        strcpy(current_clients[i], current_clients[i+1]);
    }
    nr_of_clients--;
    strcpy(current_clients[nr_of_clients], "");
}


int main(int argc, char *argv[])
{
    if (argc<1){
        printf("Podaj nr serwera\n");
        exit(-1);
    }
    int current_server_id = msgget(atoi(argv[1]), 0644|IPC_CREAT);

    int* servers_ids = load_config();
    // printf("ID kolejek przypisane serwerom:\n");
    // for (int i=1; i<=servers_ids[0]; i++){
    //     printf("%i\n", servers_ids[i]);
    // }

    // testowa obsługa rejestracji
    struct Mess reg;
    msgrcv(current_server_id, &reg, sizeof(reg)-sizeof(long), 2, 0);
    int reg_outcome = registration(reg.from_client_name);
    struct Mess reg_out;
    reg_out.msgid = 2;
    reg_out.from_server = current_server_id;
    reg_out.body[0] = reg_outcome+48;
    msgsnd(reg.from_client, &reg_out, sizeof(reg)-sizeof(long), 0);

    printf("Ilość klientów na serwerze: %i\n", nr_of_clients);
    printf("Lista klientów:\n");
    for (int i=0; i<5; i++)
        printf("%s\n", current_clients[i]);


    // initscr();
    // cbreak();
    // noecho();


    // /* 
    //     Deklaracja:
    //     Informacja o programie
    // */
    // WINDOW *server_banner_window = newwin(4, 0, 0, 0);
    // refresh();
    // box(server_banner_window, 0, 0);
    // char *project_name = "Projekt PSiW serwer 0.0.1";
    // mvwprintw(server_banner_window, 1, (COLS - strlen(project_name)) / 2, project_name);
    // boxDescription(server_banner_window, "Informacje");
    // char *authors = "Bartosz Adamczyk (148163) i Kacper Garncarek (148114)";
    // mvwprintw(server_banner_window, 2, (COLS - strlen(authors)) / 2, authors);
    // wrefresh(server_banner_window);
    

    // /* 
    //     Deklaracja:
    //     Podstawowe informacje o serwerze

    //     typu z jakimi innymi serwerami jest połączony
    //     jego adres
    // */
    // WINDOW *info_window = newwin(0, COLS/4, 5, 0);
    // refresh();
    // box(info_window, 0, 0);
    // char *adress = "Numer serwera: ";
    // mvwprintw(info_window, 2, 1, adress);
    // boxDescription(info_window, "Dane serwera");
    // wrefresh(info_window);



    // getch();
    // endwin();
    return 0;
}