#include <stdio.h>
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
#include "../../inc/server/ui.h"
#include "../../inc/server/setup.h"
#include "../../inc/server/user.h"

WINDOW *info_window;
WINDOW *server_banner_window;

int nr_of_clients = 0;
char *current_clients[5][100];
int current_server_id = 0;
time_t current_time;
int server_key = 0;
int *servers_ids;
char foo[100];

int status;
// nastepuje tu pobranie kluczy kolejek, zostalo to rozbite na dwa bo potrzebujemy listy pozostalych serwerow a nie mozna przeslac jako parametr nieokreslonego pointera

void refresh_status_client_window()
{
    sprintf(foo, "%d", nr_of_clients);
    wmove(info_window, 5, 0);
    wclrtoeol(info_window);
    mvwprintw(info_window, 5, 3, foo);
    for (int i = 0; i < 5; i++)
    {
        wmove(info_window, 7 + i, 0);
        wclrtoeol(info_window);
        mvwprintw(info_window, 7 + i, 3, current_clients[i]);
    }
    box(info_window, 0, 0);
    boxDescription(info_window, "Status serwera");

}

void setup(char *config_path)
{
    int nr_of_lines = 1;
    num_of_config_lines(&nr_of_lines, config_path);
    servers_ids = malloc(sizeof(int) * nr_of_lines + 1);
    load_config(&server_key, servers_ids, config_path);
    current_server_id = msgget(server_key, 0644 | IPC_CREAT);
    if (current_server_id == -1)
    {
        perror("Blad otwierania kolejki serwera");
    }
}
void end_of_work()
{
    msgctl(current_server_id, IPC_RMID, NULL);
    printf("Serwer zostal zamkniety\n");
    endwin();
    exit(0);
}

void window_init();

int main(int argc, char *argv[])
{
    signal(SIGINT, end_of_work);

    setup("config.in");

    // printf("Klucz serwera:%d\n", server_key);

    // tworzenie pliku logów
    char log_pathname[30];
    sprintf(log_pathname, "./logs/%d_serwer.log", server_key);
    int log_descriptor = open(log_pathname, O_CREAT | O_APPEND | O_WRONLY, 0644);
    //===========

    // cykl życia serwera
    struct Mess request, response;
    clear_mess(&request);
    clear_mess(&response);

    window_init();
    while (1)
    {
        status=msgrcv(current_server_id, &request, sizeof(request) - sizeof(long), 0, IPC_NOWAIT);
        current_time = time(NULL);
        switch (request.msgid)
        {
        case -1:
            sleep(0.05);
            break;
        case 2:                                                                                         // wymiana informacji -> rejestracja użytkownika
            int reg_outcome = registration(request.from_client_name, &current_clients, &nr_of_clients); //trzeba tutaj bledy wyeliminowac bo kompilator jakies krzaki puszcza, no chyba ze: https://preview.redd.it/u4dvwl78c5d61.jpg?auto=webp&s=f381ee6e715604cef143fe5c1c6629041b5f1c46
            request.msgid = 2;
            request.from_server = current_server_id;
            request.body[0] = reg_outcome + 48;
            msgsnd(request.from_client, &response, sizeof(response) - sizeof(long), 0);
            break;

        default: // obsluga blednego pakietu
            char foo[2048];
            sprintf(foo, "\n%s\nBlad pakietu %ld\nBody:%s\nTimestamp:%ld\nFrom client:%d\nClient name:%s\n-----------", ctime(&current_time), request.msgid, request.body, request.timestamp, request.from_client, request.from_client_name);
            write(log_descriptor, foo, strlen(foo));
            sleep(1);
            break;
        }
        refresh();

        window_init();

        clear_mess(&request);
        clear_mess(&response);
    }

    return 0;
}


void window_init(){
    endwin();
    refresh();
    initscr();
    
    /*
        Deklaracja:
        Informacja o programie
    */
    server_banner_window = newwin(4, 0, 0, 0);
    refresh();
    box(server_banner_window, 0, 0);
    char *project_name = "Projekt PSiW serwer 0.0.1";
    mvwprintw(server_banner_window, 1, (COLS - strlen(project_name)) / 2, project_name);
    boxDescription(server_banner_window, "Informacje");
    char *authors = "Bartosz Adamczyk (148163) i Kacper Garncarek (148114)";
    mvwprintw(server_banner_window, 2, (COLS - strlen(authors)) / 2, authors);

    /*
        Deklaracja:
        Podstawowe informacje o serwerze

        typu z jakimi innymi serwerami jest połączony
        jego adres
    */
    info_window = newwin(0, COLS / 4, 5, 0);
    refresh();
    box(info_window, 0, 0);
    char *adress = "Klucz serwera: ";
    mvwprintw(info_window, 2, 2, adress);
    char server_key_string[20];
    sprintf(server_key_string, "%d", server_key);
    mvwprintw(info_window, 3, 3, server_key_string);
    boxDescription(info_window, "Status serwera");
    mvwprintw(info_window, 4, 2, "Liczba klientow na serwerze: ");
    mvwprintw(info_window, 5, 3, "0");
    mvwprintw(info_window, 6, 2, "Lista klientow: ");
    refresh_status_client_window();
    wrefresh(info_window);
    wrefresh(server_banner_window);
}