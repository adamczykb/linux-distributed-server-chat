#include "../../inc/server/ui.h"
// funkcja, która umieszcza ciąg znaków na ramce wyswietlanego okienka

void window_init(struct User *user, struct Log *log, int server_key,WINDOW* background, WINDOW *server_banner_window, WINDOW *info_window, WINDOW *log_window)
{
   
    /*
        Deklaracja:
        Informacja o programie
    */
    background = newwin(0, 0, 0, 0);
    server_banner_window = newwin(4, 0, 0, 0);
    box(server_banner_window, 0, 0);
    mvwprintw(server_banner_window, 1, (COLS - strlen(TR_PROJECT_HEAD)) / 2, TR_PROJECT_HEAD);
    boxDescription(server_banner_window, TR_WIN_DESC_INFO);
    mvwprintw(server_banner_window, 2, (COLS - strlen(TR_PROJECT_AUTHORS)) / 2, TR_PROJECT_AUTHORS);

    /*
        Deklaracja:
        Podstawowe informacje o serwerze

        typu z jakimi innymi serwerami jest połączony
        jego adres
    */

    info_window = newwin(0, COLS / 4, 5, 0);
    box(info_window, 0, 0);

    mvwprintw(info_window, 2, 2, TR_SERVER_INFO_KEY);
    char server_key_string[20];
    sprintf(server_key_string, "%d", server_key);
    mvwprintw(info_window, 3, 3, server_key_string);

    mvwprintw(info_window, 4, 2, TR_SERVER_INFO_CLIENT_NUMBER);
    mvwprintw(info_window, 6, 2, TR_SERVER_INFO_CLIENT_LIST);

    /*
        Deklaracja:
        Logi
    */

    log_window = newwin(0, COLS - 2 - (COLS / 4), 5, (COLS / 4) + 2);
    wrefresh(background);

    fill_log(log, log_window);

    refresh_status_client_window(user, info_window);
    wrefresh(info_window);
    wrefresh(server_banner_window);
    wrefresh(log_window);
    
}

void fill_log(struct Log *log, WINDOW *log_window)
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
            if(log[log_index].error == 1){
                strftime(foo, 50, "[%d/%m/%Y %H:%M:%S]", localtime(&log[log_index].time));
                wattron(log_window, COLOR_PAIR(3));
                mvwprintw(log_window, i, 2, foo);
                sprintf(foo2, "<%s> %s:", "ERROR", log[log_index].head);
                mvwprintw(log_window, i, 3 + strlen(foo), foo2);
                mvwprintw(log_window, i, 4 + strlen(foo) + strlen(foo2), log[log_index].body);
                wattroff(log_window, COLOR_PAIR(3));

            }else{
                strftime(foo, 50, "[%d/%m/%Y %H:%M:%S]", localtime(&log[log_index].time));
                wattron(log_window, COLOR_PAIR(1));
                mvwprintw(log_window, i, 2, foo);
                wattroff(log_window, COLOR_PAIR(1));

                wattron(log_window, COLOR_PAIR(2));
                sprintf(foo2, "<%s> %s:", log[log_index].from, log[log_index].head);
                mvwprintw(log_window, i, 3 + strlen(foo), foo2);
                wattroff(log_window, COLOR_PAIR(2));
                mvwprintw(log_window, i, 4 + strlen(foo) + strlen(foo2), log[log_index].body);
            }
            log_index++;
        }
        else
            break;
    }
    box(log_window, 0, 0);
    boxDescription(log_window, TR_WIN_DESC_LOG);
}
void refresh_status_client_window(struct User *user, WINDOW *info_window)
{
    int current_user_num;
    char current_user_num_string[100];
    current_user_number(&current_user_num, user);
    sprintf(current_user_num_string, "%d", current_user_num);
    wmove(info_window, 5, 0);
    wclrtoeol(info_window);
    mvwprintw(info_window, 5, 3, current_user_num_string);
    for (int i = 0; i < 5; i++)
    {
        wmove(info_window, 7 + i, 0);
        wclrtoeol(info_window);
        mvwprintw(info_window, 7 + i, 3, user[i].nick);
    }
    box(info_window, 0, 0);
    boxDescription(info_window, TR_WIN_DESC_STATUS);
}
