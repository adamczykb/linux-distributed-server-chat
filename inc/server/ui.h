#include <ncurses.h>
#include <string.h>

#ifndef PSIW_UI_H
#define PSIW_UI_H

void boxDescription(WINDOW *pwin, const char *title); /* pwin (WINDOW*) -> okno ncurses, title (const char *) -> treść, która ma zostać wyświetlona nad oknem */

#endif