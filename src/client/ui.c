#include "../../inc/client/ui.h"

void init_screen(){
endwin();
    initscr();
    nodelay(stdscr, true);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_RED);
    init_pair(4, COLOR_WHITE, COLOR_MAGENTA);
}
void nick_input_screen(WINDOW *single_window,char *nick)
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
void choose_server_screen(WINDOW *single_window,int nr_of_servers, char *komunikat,int *server_keys,int *choosed_server_key)
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

    single_window = newwin(nr_of_servers + 10, 60, (LINES / 2) - (nr_of_servers + 10) / 2, (COLS / 2) - 30);
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
        for (int i = 1; i <= nr_of_servers; i++)
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
                highlight = nr_of_servers;
            break;
        case KEY_DOWN:
            highlight++;
            if (highlight > nr_of_servers)
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
    *choosed_server_key = server_keys[highlight];
}
void text_type(WINDOW *operation_window, char *text,char *alert)
{
    int x, y;
    getmaxyx(operation_window, y, x);
    char foo[1000];
    sprintf(foo, "> %s", text);
    mvwprintw(operation_window, y - 2, 2, foo);
    wattron(operation_window, COLOR_PAIR(3));
    mvwprintw(operation_window, y - 3, 2, alert);
    wattroff(operation_window, COLOR_PAIR(3));
}

void client_list(WINDOW *client_list_window, struct User *users, int n, int cursor_index[2])
{
    int i = 0;
    mvwprintw(client_list_window, 2 + i, 2, "Wybierz uzytkownika i kliknij [ENTER]");
    for (int i = 0; i < n; i++)
    {
        if (users[i].free==1)
            break;
        if (cursor_index[1] == 0 && cursor_index[0] == i)
            wattron(client_list_window, COLOR_PAIR(4));
        mvwprintw(client_list_window, 3 + i, 3, users[i].nick);
        wattroff(client_list_window, COLOR_PAIR(4));
    }
}

void channel_options(WINDOW *channel_list_window, struct Channel *channels, int n, int cursor_index[2])
{
    int i = 0, pos = 0;
    for (i = 0; i < n; i++)
    {
        if (channels[i].free)
            break;
        if (cursor_index[1] == 1 && cursor_index[0] == pos)
        {
            wattron(channel_list_window, COLOR_PAIR(4));
            mvwprintw(channel_list_window, 2 + i, 2, channels[i].name);
            wattroff(channel_list_window, COLOR_PAIR(4));
            mvwprintw(channel_list_window, 3 + i, 3, "Polaczeni uzytkownicy:");
            int j = 0;
            for (j = 0; j < num_of_users(channels[i].users, 10); j++)
            {
                mvwprintw(channel_list_window, 4 + i + j, 4, channels[i].users[j].nick);
            }
            mvwprintw(channel_list_window, 5 + i + j, 3, "[ENTER] Dolacz");
            mvwprintw(channel_list_window, 6 + i + j, 3, "[e] Opusc kanal");
            i += 4 + j;
            n += j + 4;
        }
        else
            mvwprintw(channel_list_window, 2 + i, 2, channels[i].name);
        pos++;
    }
    (++i);
    if (cursor_index[1] == 1 && cursor_index[0] == pos)
        wattron(channel_list_window, COLOR_PAIR(4));
    mvwprintw(channel_list_window, 2 + i, 2, "+ Utworz kanal");
    wattroff(channel_list_window, COLOR_PAIR(4));
}
