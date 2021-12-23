#include "../../inc/server/ui.h"
// funkcja, która umieszcza ciąg znaków na ramce wyswietlanego okienka
void boxDescription(WINDOW *pwin, const char *title)
{
    int x, stringsize;
    stringsize = 4 + strlen(title);
    x = 1;
    mvwaddch(pwin, 0, x, ' '); //mv - move, w - window, add - dodaj, ch - char
    waddstr(pwin, title);
    waddch(pwin, ' ');
}