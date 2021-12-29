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

int server_queue_id=-1;
int client_queue_id;

int choosed_server_key = 0;
int *server_keys;
char nick[100] = "";
char komunikat[100] = "";

int nr_of_serwers;

WINDOW *single_window;
// Zwraca id kolejki serwera, -1 jeśli nie ma miejsca


void choose_server_screen();
void nick_input_screen();

int connect_to_server(int server_nr, char nick[100])
{
    int server_id = msgget(server_nr, 0644);
    if(server_id==-1)
        return -1;
    struct Mess registration_msg;
    clear_mess(&registration_msg);

    registration_msg.msgid = 2;
    registration_msg.from_client = client_queue_id;

    strcpy(registration_msg.from_client_name, nick);
    msgsnd(server_id, &registration_msg, sizeof(registration_msg) - sizeof(long), 0);

    struct Mess registration_ans;
    clear_mess(&registration_ans);
    msgrcv(client_queue_id, &registration_ans, sizeof(registration_ans) - sizeof(long), 2, 0);
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
void get_server_keys(char *config_path)
{
    nr_of_serwers = 1;
    num_of_config_lines(&nr_of_serwers, config_path);
    server_keys = malloc(sizeof(int) * nr_of_serwers + 1);
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

    get_server_keys("config.in");

    while (strlen(nick) == 0)
    {
        nick_input_screen();
    }

    while (server_queue_id  == -1)
    {
        choose_server_screen();
        server_queue_id = connect_to_server(choosed_server_key, nick);

        if(server_queue_id  == -1)
            sprintf(komunikat,"Serwer %d jest pelny lub nieaktywny",choosed_server_key);
    }

    // int server_queue_id = connect_to_server(atoi(argv[1]), nick);
    // printf("ID serwera = %d\n", server_queue_id);
    wgetch(single_window);
    endwin();
    msgctl(client_queue_id, IPC_RMID, NULL);

    return 0;
}



void nick_input_screen()
{
    char *enter = "<ENTER>";
    char *foo = "Klient komunikatora";

    endwin();
    refresh();
    initscr();
    start_color();

    single_window = newwin(10, 60, (LINES / 2) - 5, (COLS / 2) - 30);
    refresh();
    box(single_window, 0, 0);
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    int y, x;
    getmaxyx(single_window, y, x);
    char input_box[x - 5];
    memset(&input_box[0], 32, sizeof(input_box));
    boxDescription(single_window, "Nazwa uzytkownika");
    mvwprintw(single_window, y - 2, (x / 2) - (strlen(enter) / 2), enter);
    mvwprintw(single_window, 2, (x / 2) - (strlen(foo) / 2), foo);
    mvwprintw(single_window, 4, 2, "Podaj swoj nick:");
    wattron(single_window, COLOR_PAIR(1));
    mvwprintw(single_window, 5, 3, input_box);
    wmove(single_window, 5, 2);
    wgetnstr(single_window, nick, 100);
    wattroff(single_window, COLOR_PAIR(1));

    wrefresh(single_window);
    mvwprintw(single_window, 3, 2, nick);
}

void choose_server_screen()
{
    endwin();
    refresh();
    initscr();
    noecho();
    cbreak();
    start_color();
    curs_set(0);
    char *enter = "\\/ <ENTER> /\\";
    char *foo = "Klient komunikatora";

    single_window = newwin(nr_of_serwers+10, 60, (LINES / 2) - (nr_of_serwers+10)/2, (COLS / 2) - 30);
    refresh();
    box(single_window, 0, 0);
    keypad(single_window, TRUE);

    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(2, COLOR_WHITE, COLOR_RED);

    int y, x;
    getmaxyx(single_window, y, x);
    boxDescription(single_window, "Wybor serwera");

    // char input_box[x-5];
    // memset(&input_box[0], 32, sizeof(input_box));
    mvwprintw(single_window, y - 2, (x / 2) - (strlen(enter) / 2), enter);
    mvwprintw(single_window, 2, (x / 2) - (strlen(foo) / 2), foo);

    wattron(single_window, COLOR_PAIR(2));
    mvwprintw(single_window, y - 3, (x / 2) - (strlen(komunikat) / 2), komunikat);
    wattroff(single_window, COLOR_PAIR(2));

    mvwprintw(single_window, 4, 2, "Wybierz serwer:");

    int choice;
    int highlight = 1;
    char temp_serwer_name[100];
    while (1)
    {
        for (int i = 1; i <= nr_of_serwers; i++)
        {
            if (i == highlight)
                wattron(single_window, COLOR_PAIR(1));
            sprintf(temp_serwer_name, "%d", server_keys[i]);
            mvwprintw(single_window, 5 + i, 3, temp_serwer_name);
            wattroff(single_window, COLOR_PAIR(1));

        }
        choice = wgetch(single_window);
        switch (choice)
        {
        case KEY_UP:
            highlight--;
            if (highlight < 1)
                highlight = nr_of_serwers;
            break;
        case KEY_DOWN:
            highlight++;
            if (highlight > nr_of_serwers)
                highlight = 1;
            break;
        default:
            break;
        }
        if (choice == 10)
        {
            break;
        }
    }
    choosed_server_key=server_keys[highlight];
}