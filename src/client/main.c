#include "../../inc/common.h"
#include "../../inc/client/communication.h"

int server_queue_id = -1;
int client_queue_id;

int choosed_server_key = 0;
int *server_keys;
char nick[100] = "";
char komunikat[100] = "";
int status;
time_t current_time;

int nr_of_serwers;

WINDOW *single_window;
WINDOW *client_banner_window;
// Zwraca id kolejki serwera, -1 jeśli nie ma miejsca

void choose_server_screen();
void nick_input_screen();
void main_screen();

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
    endwin();
    msgctl(client_queue_id, IPC_RMID, NULL);
    printf("Klient zostal zamkniety\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    int cursor_index[3]; // pierwsza zmienna to wskaznik wertkalny na liscie, drugia zmienna to wskaznik z ktorego okna, trzecia mowi jakie okno pomocnicze jest teraz otwarte
    memset(&cursor_index[0], 0, 3);

    signal(SIGINT, end_of_work);
    client_queue_id = msgget(IPC_PRIVATE, 0644 | IPC_CREAT);

    get_server_keys("config.in");

    while (strlen(nick) == 0)
    {
        nick_input_screen();
    }

    while (server_queue_id == -1)
    {
        choose_server_screen();
        connect_to_server(choosed_server_key, nick, client_queue_id, &server_queue_id);

        if (server_queue_id == -1)
            sprintf(komunikat, "Serwer %d jest pelny lub nieaktywny", choosed_server_key);
    }

    char log_pathname[30];
    sprintf(log_pathname, "./logs/%s_client.log", nick);
    int log_descriptor = open(log_pathname, O_CREAT | O_APPEND | O_WRONLY, 0644);

    if (fork() == 0)
    {
        heartbeat(client_queue_id, nick, server_queue_id);
    }

    struct Mess request, response;
    while (1)
    {
        clear_mess(&request);
        clear_mess(&response);
        status = msgrcv(client_queue_id, &request, sizeof(request) - sizeof(long), -17, IPC_NOWAIT);
        current_time = time(NULL);
        if (status == -1)
        {
            sleep(0.1);
            continue;
        }
        switch (request.msgid)
        {
        case 0:
            break;
        default: // obsluga blednego pakietu
            char foo[2048];
            sprintf(foo, "\n%s\nBlad pakietu %ld\nBody:%s\nTimestamp:%ld\nFrom client:%d\n-----------", ctime(&current_time), request.msgid, request.body, request.timestamp, request.from_server);
            write(log_descriptor, foo, strlen(foo));
            sleep(1);
            break;
        }
        refresh();
        main_screen();
    }

    endwin();
    msgctl(client_queue_id, IPC_RMID, NULL);

    return 0;
}

void main_screen()
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
    client_banner_window = newwin(4, 0, 0, 0);
    refresh();
    box(client_banner_window, 0, 0);
    char *project_name = "Projekt PSiW serwer 0.0.1";
    mvwprintw(client_banner_window, 1, (COLS - strlen(project_name)) / 2, project_name);
    boxDescription(client_banner_window, "Informacje");
    char *authors = "Bartosz Adamczyk (148163) i Kacper Garncarek (148114)";
    mvwprintw(client_banner_window, 2, (COLS - strlen(authors)) / 2, authors);
}
void nick_input_screen()
{
    char *enter = "< ENTER >";
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
    char *enter = "\\/ < ENTER > /\\";
    char *foo = "Klient komunikatora";

    single_window = newwin(nr_of_serwers + 10, 60, (LINES / 2) - (nr_of_serwers + 10) / 2, (COLS / 2) - 30);
    refresh();
    box(single_window, 0, 0);
    keypad(single_window, TRUE);

    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(2, COLOR_WHITE, COLOR_RED);

    int y, x;
    getmaxyx(single_window, y, x);
    boxDescription(single_window, "Wybor serwera");

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
    choosed_server_key = server_keys[highlight];
}

