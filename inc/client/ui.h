#include <ncurses.h>
#include <string.h>

#ifndef PSIW_USER_UI_H
#define PSIW_USER_UI_H

void nick_input_screen(WINDOW *single_window,char *nick);
void choose_server_screen(WINDOW *single_window,int nr_of_servers, char *komunikat,int *server_keys,int *choosed_server_key);
#endif