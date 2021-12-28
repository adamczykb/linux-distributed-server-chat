#include "../../inc/server/user.h"

void registration(char nickname[100],  char *current_clients[][100], int *nr_of_clients, int *status)
{
    // Sprawdzenie czy nazwa już nie występuje
    for (int i = 0; i < 5; i++)
    {
        if (!strcmp(current_clients[i], nickname))
        {
            *status = -1;
            return;
        }
    }
    // Dodanie użytkownika
    strcpy(current_clients[*nr_of_clients], nickname);
    (*nr_of_clients)++;
    *status=0;
    return;
}

void logout(char nickname[100],  char *current_clients[][100], int *nr_of_clients, int *status)
{
    int placement;
    // Sprawdzenie gdzie na liście występuje ta nazwa
    for (int i = 0; i < *nr_of_clients; i++)
    {
        if (strcmp(current_clients[i], nickname))
        {
            placement = i;
            break;
        }
    }
    // Usunięcie użytkownika i przesunięcie pozostałych nazw
    for (int i = placement; i < *nr_of_clients; i++)
    {
        strcpy(current_clients[i], current_clients[i + 1]);
    }
    (*nr_of_clients)--;
    strcpy(current_clients[*nr_of_clients], "");
}
