#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void boxDescription(WINDOW *pwin, const char *title)
{
    int x, stringsize;
    stringsize = 4 + strlen(title);
    x = 1;
    mvwaddch(pwin, 0, x, ' ');
    waddstr(pwin, title);
    waddch(pwin, ' ');
}

// Zwraca wskaźnik na tablicę [ilość_serwerów, id_kolejki_serwera1, ...]
int* load_config(){
    // odczytanie pliku
    char* filename = "config.txt";
    int file = open(filename, O_RDWR);
    char buff[100] = "";
    int nr_of_lines = 1;
    for (int i=0; read(file, &buff[i], 1)>0; i++)
        if (buff[i-1]=='\n' && buff[i]>=48 && buff[i]<=57) nr_of_lines += 1;
    close(file);

    // zapisanie elementów z pliku do listy
    int *queue_ids = (int *)malloc(sizeof(int)*nr_of_lines+1);
    queue_ids[0] = nr_of_lines;
    int current_v=0, current_i=1;
    for (int i=0; i<100; i++){
        if (buff[i]=='\0') break;
        if (buff[i]=='\n'){
            queue_ids[current_i] = current_v;
            current_v = 0;
            current_i += 1;
            continue;
        }
        current_v = current_v*10+buff[i]-48;
    }
    if (current_v != 0) queue_ids[current_i] = current_v;
    return queue_ids;
}

int main(int argc, char *argv[])
{
    int* ids = load_config();
    printf("Zawartość config.txt:\n");
    for (int i=1; i<=ids[0]; i++){
        printf("%i\n", ids[i]);
    }

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
    char *adress = "Numer serwera: ";
    mvwprintw(info_window, 2, 1, adress);
    boxDescription(info_window, "Dane serwera");
    wrefresh(info_window);



    getch();
    endwin();
    return 0;
}