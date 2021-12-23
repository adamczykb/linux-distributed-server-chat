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

int nr_of_clients = 0;
char *current_clients[5][100];
int current_server_id;
time_t current_time;
int server_key = 0;

void end_of_work()
{
    msgctl(current_server_id, IPC_RMID, NULL);
    printf("Serwer zostal zamkniety\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, end_of_work);
    int *servers_ids;
    load_config(&server_key, servers_ids, "config.txt");
    // initscr();
    // cbreak();
    // noecho();

    current_server_id = msgget(server_key, 0644 | IPC_CREAT);
    printf("Klucz serwera:%d\n", server_key);

    // tworzenie pliku logów
    char log_pathname[30];
    sprintf(log_pathname, "./logs/%d_serwer.log", server_key);
    int log_descriptor = open(log_pathname, O_CREAT | O_APPEND | O_WRONLY, 0777);
    //===========

    // cykl życia serwera
    struct Mess request, response;
    clear_mess(&request);
    clear_mess(&response);
    while (1)
    {
        msgrcv(current_server_id, &request, sizeof(request) - sizeof(long), 0, 0);
        current_time = time(NULL);
        switch (request.msgid)
        {

        case 2: // wymiana informacji -> rejestracja użytkownika
            int reg_outcome = registration(request.from_client_name, &current_clients, &nr_of_clients); //trzeba tutaj bledy wyeliminowac bo kompilator jakies krzaki puszcza, no chyba ze: https://preview.redd.it/u4dvwl78c5d61.jpg?auto=webp&s=f381ee6e715604cef143fe5c1c6629041b5f1c46
            request.msgid = 2;
            request.from_server = current_server_id;
            request.body[0] = reg_outcome + 48;
            msgsnd(request.from_client, &response, sizeof(response) - sizeof(long), 0);

            printf("Ilość klientów na serwerze: %i\n", nr_of_clients);
            printf("Lista klientów:\n");
            for (int i = 0; i < 5; i++)
                printf("%s\n", current_clients[i]);

            break;

        default: // obsluga blednego pakietu
            char foo[2048];
            sprintf(foo, "\n%s\nBlad pakietu %ld\nBody:%s\nTimestamp:%ld\nFrom client:%d\nClient name:%s\n-----------", ctime(&current_time), request.msgid, request.body, request.timestamp, request.from_client, request.from_client_name);
            write(log_descriptor, foo, strlen(foo));
            sleep(1);
            break;
        }
        clear_mess(&request);
        clear_mess(&response);
    }

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