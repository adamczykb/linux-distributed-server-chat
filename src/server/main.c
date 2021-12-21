#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

void boxDescription(WINDOW *pwin, const char *title)
{
    int x, stringsize;
    stringsize = 4 + strlen(title);
    x = 1;
    mvwaddch(pwin, 0, x, ' ');
    waddstr(pwin, title);
    waddch(pwin, ' ');
}

int main(int argc, char *argv[])
{
    initscr();
    cbreak();
    noecho();


    /* 
        Deklaracja:
        Informacja o programie
    */
    WINDOW *server_banner_window = newwin(4, 0, 0, 0);
    refresh();
    box(server_banner_window, 0, 0);
    char *project_name = "Projekt PSiW serwer 0.0.1";
    mvwprintw(server_banner_window, 1, (COLS - strlen(project_name)) / 2, project_name);
    boxDescription(server_banner_window, "Informacje");
    char *authors = "Bartosz Adamczyk (148163) i Kacper Garncarek (148114)";
    mvwprintw(server_banner_window, 2, (COLS - strlen(authors)) / 2, authors);
    wrefresh(server_banner_window);
    

    /* 
        Deklaracja:
        Podstawowe informacje o serwerze

        typu z jakimi innymi serwerami jest połączony
        jego adres
    */
    WINDOW *info_window = newwin(0, COLS/4, 5, 0);
    refresh();
    box(info_window, 0, 0);
    char *adress = "Numer serwera: 2";
    mvwprintw(info_window, 2, 1, adress);
    boxDescription(info_window, "Dane serwera");
    wrefresh(info_window);



    getch();
    endwin();
    return 0;
}