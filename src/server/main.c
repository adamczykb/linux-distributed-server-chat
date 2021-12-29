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
WINDOW *log_window;

struct Log log[500];

int nr_of_clients = 0;
char *current_clients[5][100];
int current_server_id = 0;
time_t current_time;
int server_key = 0;
int *servers_ids;
char foo[100];
int status;

// nastepuje tu pobranie kluczy kolejek, zostalo to rozbite na dwa bo potrzebujemy listy pozostalych serwerow a nie mozna przeslac jako parametr nieokreslonego pointera

void setup(char *config_path)
{
    int nr_of_lines = 1;
    num_of_config_lines(&nr_of_lines, config_path);
    servers_ids = malloc(sizeof(int) * nr_of_lines + 1);
    load_config(&server_key, servers_ids, config_path);
    current_server_id = msgget(server_key, 0644 | IPC_CREAT);
    init_log(log);
    

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

void refresh_status_client_window();
void window_init();
void fill_log();

int main(int argc, char *argv[])
{
    signal(SIGINT, end_of_work);

    setup("config.in");

    // tworzenie pliku logów
    char log_pathname[30];
    sprintf(log_pathname, "./logs/%d_serwer.log", server_key);
    int log_descriptor = open(log_pathname, O_CREAT | O_APPEND | O_WRONLY, 0644);
    //===========

    // cykl życia serwera
    struct Mess request, response;
    clear_mess(&request);
    clear_mess(&response);
    add_to_log(log, time(NULL), "Serwer wystartowal", "localhost", "gotowy na odbior wiadomosci");
    window_init();
    while (1)
    {
        status = msgrcv(current_server_id, &request, sizeof(request) - sizeof(long), 0, IPC_NOWAIT);
        current_time = time(NULL);
        switch (request.msgid)
        {
        case -1:
            sleep(0.05);
            break;
        case 2: // wymiana informacji -> rejestracja użytkownika
            int reg_outcome;
            registration(request.from_client_name, &current_clients, &nr_of_clients, &reg_outcome); //trzeba tutaj bledy wyeliminowac bo kompilator jakies krzaki puszcza, no chyba ze: https://preview.redd.it/u4dvwl78c5d61.jpg?auto=webp&s=f381ee6e715604cef143fe5c1c6629041b5f1c46
            response.msgid = 2;
            response.from_server = current_server_id;
            response.body[0] = reg_outcome + 48;
            msgsnd(request.from_client, &response, sizeof(response) - sizeof(long), 0);

            char from_client_string[20];
            sprintf(from_client_string,"%d",request.from_client);
            add_to_log(log, time(NULL), "Dolaczyl uzytkownik", from_client_string, request.from_client_name);
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
    msgctl(current_server_id, IPC_RMID, NULL);
    return 0;
}

void window_init()
{
    endwin();
    refresh();
    initscr();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
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
    mvwprintw(info_window, 4, 2, "Liczba klientow na serwerze: ");
    mvwprintw(info_window, 6, 2, "Lista klientow: ");

    /*
        Deklaracja:
        Logi
    */
    log_window = newwin(0, COLS - 2 - (COLS / 4), 5, (COLS / 4) + 2);
    refresh();

    fill_log();

    refresh_status_client_window();
    wrefresh(info_window);
    wrefresh(server_banner_window);
    wrefresh(log_window);
}

void fill_log()
{
    wmove(log_window, 1, 1);
    wclrtobot(log_window);
    int x, y;
    getmaxyx(log_window, y, x);
    char foo[50], foo2[200];
    int log_index = 0;

    for (int i = y - 2; i > 0; i--)
    {
        if (log[log_index].empty == 0)
        {
            strftime(foo, 50, "[%d/%m/%Y %H:%M:%S]", localtime(&log[log_index].time));
            wattron(log_window, COLOR_PAIR(1));
            mvwprintw(log_window, i, 2, foo);
            wattroff(log_window, COLOR_PAIR(1));

            wattron(log_window, COLOR_PAIR(2));
            sprintf(foo2, "<%s> %s:", log[log_index].from, log[log_index].head);
            mvwprintw(log_window, i, 3 + strlen(foo), foo2);
            wattroff(log_window, COLOR_PAIR(2));
            mvwprintw(log_window, i, 4 + strlen(foo) + strlen(foo2), log[log_index].body);

            log_index++;
        }
        else
            break;
    }
    box(log_window, 0, 0);
    boxDescription(log_window, "Zmiany");
}
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