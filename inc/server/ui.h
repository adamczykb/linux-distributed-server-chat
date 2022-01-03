#include <ncurses.h>
#include <string.h>
#include "user.h"
#include "logger.h"
#include "trans.h"

#ifndef PSIW_UI_H
#define PSIW_UI_H

void window_init(struct User (*user)[MAX_USER], struct Log *log, int server_key, WINDOW * background ,WINDOW *server_banner_window, WINDOW *info_window, WINDOW *log_window);
void fill_log(struct Log *log, WINDOW *log_window);
void refresh_status_client_window(struct User (*user)[MAX_USER], WINDOW *info_window);

#endif