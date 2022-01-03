#include "../../inc/client/ui.h"

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
