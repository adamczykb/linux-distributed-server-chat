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
char text[MAX_MSG_LEN] = "TEST";

struct User{
    int queue_id;
    char nick[100];
    int free; // 0 -> nie 1 -> tak
};

struct Channel{
    int queue_id;
    char name[100];
    struct User users[10];
    struct Mess messages[100]; //wiem ze mialo byc 10 ale mysle ze 10 ma przysylac serwer
    int free; // 0 -> nie 1 -> tak
};

struct User klients[50];
struct Channel channels[50];

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

void init_user_struct(struct User *user,int n)
{
    for (int i = 0; i < n; i++)
    {
        user[i].free = 1;
        user[i].queue_id = 0;
        strcpy(user[i].nick, "");
    }
     user[0].free=0;
    user[0].queue_id = 0;
    strcpy(user[0].nick, "LOL");
     user[1].free=0;
    user[1].queue_id = 0;
    strcpy(user[1].nick, "LOL2");
}

int num_of_users(struct User *users,int n){
    int i=0;
    for (i=0;i<n;i++){
        if(users[i].free)
            break;
    }
    return i;
}

int num_of_channels(struct Channel *channels,int n){
    int i=0;
    for (i=0;i<n;i++){
        if(channels[i].free)
            break;
    }
    return i;
}

void init_channel_struct(struct Channel *channel,int n)
{
    for (int i = 0; i < n; i++)
    {
        channel[i].free = 1;
        channel[i].queue_id = 0;
        strcpy(channel[i].name, "");
        init_user_struct(channel->users,10);
        for(int j=0;j<100;j++){
            clear_mess(&channel[i].messages[j]);
        }
    }
    channel[0].free=0;
    channel[0].queue_id = 0;
    strcpy(channel[0].name, "LOL");
    strcpy(channel[0].users[0].nick,"Jan");
    channel[0].users[0].free=0;
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
    endwin();
    msgctl(client_queue_id, IPC_RMID, NULL);
    printf("Klient zostal zamkniety\n");
    exit(0);
}

void client_list(WINDOW *client_list_window,struct User *users,int n,int cursor_index[2]){
    int i=0;
    mvwprintw(client_list_window, 2+i, 2, "Wybierz uzytkownika i kliknij [ENTER]");
    for(int i =0;i<n;i++){
        if(users[i].free)
            break;
        if(cursor_index[1]==0 && cursor_index[0]==i)
            wattron(client_list_window, COLOR_PAIR(4));
        mvwprintw(client_list_window, 3+i, 3, users[i].nick);
        wattroff(client_list_window, COLOR_PAIR(4));

    }

}

void channel_options(WINDOW *channel_list_window,struct Channel *channels,int n,int cursor_index[2]){
    int i=0,pos=0;
    for(i =0;i<n;i++){
        if(channels[i].free)
            break;
        if(cursor_index[1]==1 && cursor_index[0]==pos){
            wattron(channel_list_window, COLOR_PAIR(4));
            mvwprintw(channel_list_window, 2+i, 2, channels[i].name);
            wattroff(channel_list_window, COLOR_PAIR(4));
            mvwprintw(channel_list_window, 3+i, 3, "Polaczeni uzytkownicy:");
            int j=0;
            for(j=0;j< num_of_users(channels[i].users,10);j++){
                mvwprintw(channel_list_window, 4+i+j, 4, channels[i].users[j].nick);    
            }
            mvwprintw(channel_list_window, 5+i+j, 3, "[ENTER] Dolacz");
            mvwprintw(channel_list_window, 6+i+j, 3, "[e] Opusc kanal");
            i+=4+j;
            n+=j+4;
        }else
            mvwprintw(channel_list_window, 2+i, 2, channels[i].name);
        pos++;

    }
    (++i);
    if(cursor_index[1]==1 && cursor_index[0]==pos)
        wattron(channel_list_window, COLOR_PAIR(4));
    mvwprintw(channel_list_window, 2+i, 2, "+ Utworz kanal");
    wattroff(channel_list_window, COLOR_PAIR(4));

}
void text_type(WINDOW *operation_window,char *text){
    int x,y;
    getmaxyx(operation_window,y,x);
    char foo[1000];
    sprintf(foo,"> %s",text);
    mvwprintw(operation_window, y-2, 2, foo);
    wattron(operation_window,COLOR_PAIR(3));
    mvwprintw(operation_window, y-3, 2, alert);
    wattroff(operation_window,COLOR_PAIR(3));

}

int main(int argc, char *argv[])
{
    // pierwsza zmienna to wskaznik wertkalny na liscie, drugia zmienna to wskaznik z ktorego okna, trzecia mowi jakie okno pomocnicze jest teraz otwarte
    cursor_index[0]=cursor_index[1]=0;
    signal(SIGINT, end_of_work);
    client_queue_id = msgget(IPC_PRIVATE, 0644 | IPC_CREAT);

    get_server_keys("config.in");
    init_user_struct(klients,50);
    init_channel_struct(channels,50);

    while (strlen(nick) == 0)
    {
        nick_input_screen(single_window,&nick);
    }

    while (server_queue_id == -1)
    {
        choose_server_screen(single_window,nr_of_serwers, alert,server_keys,&choosed_server_key);
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

    struct Mess request, response;
    strcpy(alert,"");
    endwin();
    initscr();
    nodelay(stdscr, true);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_BLACK,COLOR_RED);
    init_pair(4, COLOR_WHITE, COLOR_MAGENTA);
    while (1)
    {
        main_screen();
        

        clear_mess(&request);
        clear_mess(&response);
        status = msgrcv(client_queue_id, &response, sizeof(response) - sizeof(long), -17, IPC_NOWAIT);
        current_time = time(NULL);
        if (status == -1)
        {
            usleep(50*1000); // 20fps
            continue;
        }
        switch (response.msgid)
        {
        case 0:
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
        if (cursor_index[1]==0 && cursor_index[0] < 0)
            cursor_index[0] = num_of_users(klients,50)-1;
        if (cursor_index[1]==1 && cursor_index[0] < 0)
            cursor_index[0] = num_of_channels(channels,50);
        break;
    case KEY_DOWN:
        cursor_index[0]++;
        if (cursor_index[1]==0 && cursor_index[0] >= num_of_users(klients,50))
            cursor_index[0] = 0;
        if (cursor_index[1]==1 && cursor_index[0] > num_of_channels(channels,50))
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
        if(cursor_index[1]==2)
            strcpy(text,"");
        strcpy(alert,"COS");
        break;
    case KEY_BACKSPACE:
        if(choice==8 && strlen(text)>0 && cursor_index[1]==2){
            text[strlen(text)-1]=0;
        }
        break;
    default:
        if (choice>31 && choice <127 && strlen(text)<MAX_MSG_LEN && cursor_index[1]==2){
            sprintf(text,"%s%c",text,choice);
        }
        if((choice==8||choice==127) && strlen(text)>0 && cursor_index[1]==2){
            text[strlen(text)-1]=0;
        }
        break;
    }
   
    /*
        Deklaracja:
        Informacja o programie
    */
    background=newwin(0, 0, 0, 0);
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
    info_window = newwin(10, (COLS/4), 5, 0);
    box(info_window, 0, 0);
    boxDescription(info_window, "Status");
    mvwprintw(info_window, 2, 2, "ID klienta: ");
    char client_queue_id_string[10];
    sprintf(client_queue_id_string,"%d",client_queue_id);
    mvwprintw(info_window, 3, 3, client_queue_id_string);
    mvwprintw(info_window, 4, 2, "Nick klienta: ");
    mvwprintw(info_window, 5, 3, nick);
    char server_name[10];
    sprintf(server_name,"%d",choosed_server_key);
    mvwprintw(info_window, 6, 2, "Podlaczony do serwera: ");
    mvwprintw(info_window, 7, 3, server_name);
    
    /*
        Deklaracja:
        Lista dostepnych klientow
    */
    client_list_window = newwin((LINES-17)/2, (COLS/4), 16, 0);
    if(cursor_index[1]==0)
        wattron(client_list_window, COLOR_PAIR(2));
    box(client_list_window, 0, 0);
    wattroff(client_list_window, COLOR_PAIR(2));
    boxDescription(client_list_window, "Lista klientow");
    client_list(client_list_window,klients,50,cursor_index);
    /*
        Deklaracja:
        Lista dostepnych kanalow
    */
    channel_list_window = newwin(0, (COLS/4), 17+(LINES-17)/2, 0);
    if(cursor_index[1]==1)
        wattron(channel_list_window, COLOR_PAIR(2));
    box(channel_list_window, 0, 0);
    wattroff(channel_list_window, COLOR_PAIR(2));
    boxDescription(channel_list_window, "Lista kanalow");
    channel_options(channel_list_window,channels,50,cursor_index);

    /*
        Deklaracja:
        Okno operacyjne
    */
    operation_window = newwin(LINES-5, 3*(COLS/4)-2, 5, (COLS/4)+1);
    if(cursor_index[1]==2)
        wattron(operation_window, COLOR_PAIR(2));
    box(operation_window, 0, 0);
    wattroff(operation_window, COLOR_PAIR(2));
    boxDescription(operation_window, "");
    text_type(operation_window,text);

    wrefresh(background);
    wrefresh(channel_list_window);
    wrefresh(operation_window);
    wrefresh(client_banner_window);
    wrefresh(client_list_window);
    wrefresh(info_window);
}



