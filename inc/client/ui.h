#include <ncurses.h>
#include <string.h>
#include "utils.h"

#ifndef PSIW_USER_UI_H
#define PSIW_USER_UI_H
void init_screen();
void nick_input_screen(WINDOW *single_window,char *nick);
void choose_server_screen(WINDOW *single_window,int nr_of_servers, char *komunikat,int *server_keys,int *choosed_server_key);
void text_type(WINDOW *operation_window, char *text,char *alert);
void channel_options(WINDOW *channel_list_window, struct Channel *channels, int n, int cursor_index[2]);
#endif