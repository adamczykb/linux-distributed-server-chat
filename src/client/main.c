#include "../../inc/common.h"
#include "../../inc/client/communication.h"
#include "../../inc/client/ui.h"
#define MAX_MSG_LEN 100
int server_queue_id = -1;
int client_queue_id;

int choosed_server_key = 0;
int *server_keys;
char nick[100] = "";
char op_window_title[100] = "Komunikator";
char input_text[MAX_MSG_LEN] = "";

enum typing_target input_target;
enum message_type mess_type;
int target_id = 0;    // kod klienta docelowego lub kanalu
int target_index = 0; // indeks dla tablicy struktu

struct User clients[50];
struct Channel channels[50];
struct Mess request, response;

struct Mess request, response;

int cursor_index[3];

char alert[100] = "";
int status;
time_t current_time;

int nr_of_serwers;

WINDOW *single_window;
WINDOW *client_banner_window;
WINDOW *client_list_window;
WINDOW *channel_list_window;
WINDOW *info_window;
WINDOW *operation_window;
WINDOW *background;
// Zwraca id kolejki serwera, -1 jeśli nie ma miejsca

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

void load_messages_to_window(WINDOW *operation_window, struct Mess *mess)
{
    int x, y;

    getmaxyx(operation_window, y, x);
    for (int i = 0; i < 100; i++)
    {
        char foo[255], foo2[255];
        if (strlen(mess[i].from_client_name) == 0 || i > y - 4)
            break;
        strftime(foo, 50, "[%d/%m/%Y %H:%M:%S]", localtime(&mess[i].timestamp));
        wattron(operation_window, COLOR_PAIR(1));
        mvwprintw(operation_window, y - 4 - i, 2, foo);
        wattroff(operation_window, COLOR_PAIR(1));

        wattron(operation_window, COLOR_PAIR(2));
        sprintf(foo2, "<%s> %s:", mess[i].from_client, mess[i].from_client_name);
        mvwprintw(operation_window, y - 4 - i, 3 + strlen(foo), foo2);
        wattroff(operation_window, COLOR_PAIR(2));
        mvwprintw(operation_window, y - 4 - i, 4 + strlen(foo) + strlen(foo2), mess[i].body);
    }
}

int main(int argc, char *argv[])
{
    input_target = TYPING_TARGET_NO_DECLARED;
    mess_type = MSG_TYPE_NO_DECLARED;
    // pierwsza zmienna to wskaznik wertkalny na liscie, drugia zmienna to wskaznik z ktorego okna, trzecia mowi jakie okno pomocnicze jest teraz otwarte
    cursor_index[0] = cursor_index[1] = 0;
    signal(SIGINT, end_of_work);
    client_queue_id = msgget(IPC_PRIVATE, 0644 | IPC_CREAT);

    get_server_keys("config.in");
    init_user_struct(clients, 50);
    init_channel_struct(channels);

    while (strlen(nick) == 0)
    {
        nick_input_screen(single_window, &nick);
    }

    while (server_queue_id == -1)
    {
        choose_server_screen(single_window, nr_of_serwers, alert, server_keys, &choosed_server_key);
        connect_to_server(choosed_server_key, nick, client_queue_id, &server_queue_id);

        if (server_queue_id == -1)
            sprintf(alert, "Serwer %d jest pelny lub nieaktywny", choosed_server_key);
    }

    char log_pathname[30];
    sprintf(log_pathname, "./logs/%s_client.log", nick);
    int log_descriptor = open(log_pathname, O_CREAT | O_APPEND | O_WRONLY, 0644);

    if (fork() == 0)
    {
        heartbeat(client_queue_id, nick, server_queue_id);
    }
    strcpy(alert, "");
    init_screen();
    while (1)
    {
        main_screen();

        clear_mess(&request);
        clear_mess(&response);
        status = msgrcv(client_queue_id, &response, sizeof(response) - sizeof(long), -17, IPC_NOWAIT);
        current_time = time(NULL);
        if (status == -1)
        {
            usleep(50 * 1000); // 20fps
            continue;
        }
        switch (response.msgid)
        {
        case 0:
            break;
        case 3:
            new_channel(channels,&response);
            break;
        default: // obsluga blednego pakietu
            char foo[2048];
            sprintf(foo, "\n%s\nBlad pakietu %ld\nBody:%s\nTimestamp:%ld\nFrom client:%d\n-----------", ctime(&current_time), response.msgid, response.body, response.timestamp, response.from_server);
            write(log_descriptor, foo, strlen(foo));
            sleep(1);
            break;
        }
    }

    endwin();
    msgctl(client_queue_id, IPC_RMID, NULL);

    return 0;
}

void main_screen()
{

    int choice = getch();
    switch (choice)
    {
    case KEY_UP:
        cursor_index[0]--;
        if (cursor_index[1] == 0 && cursor_index[0] < 0)
            cursor_index[0] = num_of_users(clients, 50) - 1;
        if (cursor_index[1] == 1 && cursor_index[0] < 0)
            cursor_index[0] = num_of_channels(channels);
        break;
    case KEY_DOWN:
        cursor_index[0]++;
        if (cursor_index[1] == 0 && cursor_index[0] >= num_of_users(clients, 50))
            cursor_index[0] = 0;
        if (cursor_index[1] == 1 && cursor_index[0] > num_of_channels(channels))
            cursor_index[0] = 0;
        break;
    case KEY_LEFT:
        cursor_index[1]--;
        if (cursor_index[1] < 0)
            cursor_index[1] = 2;
        break;
    case KEY_RIGHT:
        cursor_index[1]++;
        if (cursor_index[1] > 2)
            cursor_index[1] = 0;
        break;
    case '\n':
        if (cursor_index[1] == 2)
        {
            switch (input_target)
            {
            case TYPING_TARGET_NO_DECLARED:
                strcpy(input_text, "");
                break;
            case TYPING_TARGET_NEW_CHANNEL:
                //tu bedzie lecial pakiet do serwera i czekal na odpowiedz i od razu otworzy okno wiadomosci
                clear_mess(&request);
                clear_mess(&response);
                
                request.msgid = 3;
                request.from_client = client_queue_id;
                request.timestamp = time(NULL);
                strcpy(request.from_client_name, nick);
                strcpy(request.body, input_text);
                strcpy(input_text, "");
                msgsnd(server_queue_id, &request, sizeof(request) - sizeof(long), 0);
                strcpy(alert, "");

                msgrcv(client_queue_id, &response, sizeof(response) - sizeof(long), 3, 0);
                new_channel(channels,&response);

                input_target = TYPING_TARGET_NEW_MESSAGE;
                mess_type = MSG_TYPE_CHANNEL;
 
                target_id = channels[num_of_channels(channels)-1].id;
                target_index = num_of_channels(channels)-1;
                cursor_index[1] = 2;
                cursor_index[0]=num_of_channels(channels)-1;

                sprintf(op_window_title,"Rozmowa na kanale %s",channels[cursor_index[0]].name);
                break;
            case TYPING_TARGET_NEW_MESSAGE:
                //tu bedzie lecial pakiet do serwera
                break;
            default:
                break;
            }
        }
        if (cursor_index[1] == 0 && cursor_index[0] < num_of_users(clients,50))
        {
            strcpy(input_text, "");
            input_target = TYPING_TARGET_NEW_MESSAGE;
            mess_type = MSG_TYPE_DIRECT;
            target_id = clients[cursor_index[0]].queue_id;
            target_index = cursor_index[0];
            cursor_index[1] = 2;
            sprintf(op_window_title,"Rozmowa z %s",clients[cursor_index[0]].nick);


        }
        if (cursor_index[1] == 1 && cursor_index[0] < num_of_channels(channels))
        {
            strcpy(input_text, "");
            input_target = TYPING_TARGET_NEW_MESSAGE;
            mess_type = MSG_TYPE_CHANNEL;
            target_id = channels[cursor_index[0]].id;
            target_index = cursor_index[0];
            cursor_index[1] = 2;
            sprintf(op_window_title,"Rozmowa na kanale %s",channels[cursor_index[0]].name);
        }
        if (cursor_index[1] == 1 && cursor_index[0] == num_of_channels(channels))
        {
            strcpy(input_text, "");
            input_target = TYPING_TARGET_NEW_CHANNEL;
            mess_type = MSG_TYPE_NO_DECLARED;
            cursor_index[1] = 2;
            strcpy(op_window_title,"Tworzenie nowego kanalu");
            strcpy(alert, "Podaj nazwe tworzonego kanalu!");
        }
        break;
    case 8:
    case 127:
    case KEY_BACKSPACE:
        if (strlen(input_text) > 0 && cursor_index[1] == 2)
            input_text[strlen(input_text) - 1] = 0;
        break;
    default:
        if (choice > 31 && strlen(input_text) < MAX_MSG_LEN && cursor_index[1] == 2)
        {
            sprintf(input_text, "%s%c", input_text, choice);
        }
        break;
    }

    /*
        Deklaracja:
        Informacja o programie
    */
    background = newwin(0, 0, 0, 0);
    client_banner_window = newwin(4, 0, 0, 0);
    box(client_banner_window, 0, 0);
    char *project_name = "'Projekt PSiW' klient 0.0.1";
    mvwprintw(client_banner_window, 1, (COLS - strlen(project_name)) / 2, project_name);
    boxDescription(client_banner_window, "Informacje");
    char *authors = "Bartosz Adamczyk (148163) i Kacper Garncarek (148114)";
    mvwprintw(client_banner_window, 2, (COLS - strlen(authors)) / 2, authors);

    /*
        Deklaracja:
        Szczegoly klienta
    */
    info_window = newwin(10, (COLS / 4), 5, 0);
    box(info_window, 0, 0);
    boxDescription(info_window, "Status");
    mvwprintw(info_window, 2, 2, "ID klienta: ");
    char client_queue_id_string[10];
    sprintf(client_queue_id_string, "%d", client_queue_id);
    mvwprintw(info_window, 3, 3, client_queue_id_string);
    mvwprintw(info_window, 4, 2, "Nick klienta: ");
    mvwprintw(info_window, 5, 3, nick);
    char server_name[10];
    sprintf(server_name, "%d", choosed_server_key);
    mvwprintw(info_window, 6, 2, "Podlaczony do serwera: ");
    mvwprintw(info_window, 7, 3, server_name);

    /*
        Deklaracja:
        Lista dostepnych klientow
    */
    client_list_window = newwin((LINES - 17) / 2, (COLS / 4), 16, 0);
    if (cursor_index[1] == 0)
        wattron(client_list_window, COLOR_PAIR(2));
    box(client_list_window, 0, 0);
    wattroff(client_list_window, COLOR_PAIR(2));

    clear_mess(&request);
    request.msgid = 7;
    request.from_client = client_queue_id;
    msgsnd(server_queue_id, &request, sizeof(request)-sizeof(long), 0);

    clear_mess(&response);
    msgrcv(client_queue_id, &response, sizeof(response)-sizeof(long), 7, 0);
    printf(response.body);

    boxDescription(client_list_window, "Lista klientow");
    client_list(client_list_window, clients, 50, cursor_index);
    /*
        Deklaracja:
        Lista dostepnych kanalow
    */
    channel_list_window = newwin(0, (COLS / 4), 17 + (LINES - 17) / 2, 0);
    if (cursor_index[1] == 1)
        wattron(channel_list_window, COLOR_PAIR(2));
    box(channel_list_window, 0, 0);
    wattroff(channel_list_window, COLOR_PAIR(2));
    boxDescription(channel_list_window, "Lista kanalow");
    channel_options(channel_list_window, channels, 50, cursor_index);

    /*
        Deklaracja:
        Okno operacyjne
    */
    operation_window = newwin(LINES - 5, 3 * (COLS / 4) - 2, 5, (COLS / 4) + 1);
    if (cursor_index[1] == 2)
        wattron(operation_window, COLOR_PAIR(2));
    box(operation_window, 0, 0);
    wattroff(operation_window, COLOR_PAIR(2));
    boxDescription(operation_window, op_window_title);
    text_type(operation_window, input_text, alert);
    if (input_target == TYPING_TARGET_NEW_MESSAGE)
    {
        if (mess_type == MSG_TYPE_DIRECT)
        {
            load_messages_to_window(operation_window, clients[target_index].messages);
        }
        if (mess_type == MSG_TYPE_CHANNEL)
        {
            load_messages_to_window(operation_window, channels[target_index].messages);
        }
    }
    wrefresh(background);
    wrefresh(channel_list_window);
    wrefresh(operation_window);
    wrefresh(client_banner_window);
    wrefresh(client_list_window);
    wrefresh(info_window);
}
