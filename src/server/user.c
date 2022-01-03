#include "../../inc/server/user.h"

void init_user_struct(struct User (*user)[MAX_USER])
{
    for (int i = 0; i < MAX_USER; i++)
    {
        user[i]->free = 1;
        user[i]->queue_id = 0;
        strcpy(user[i]->nick, "");
    }
}

void registration(int queue_id, char nickname[100], struct User (*user)[MAX_USER], int *status)
{
    int nr_of_clients;
    current_user_number(&nr_of_clients, user);
    if (nr_of_clients > 4)
    {
        *status = -1;
        return;
    }
    // Sprawdzenie czy nazwa już nie występuje
    for (int i = 0; i < MAX_USER; i++)
    {
        if (!strcmp(user[i]->nick, nickname))
        {
            *status = -1;
            return;
        }
    }
    // Dodanie użytkownika
    strcpy(user[nr_of_clients]->nick, nickname);
    user[nr_of_clients]->queue_id = queue_id;
    user[nr_of_clients]->free = 0;
    *status = 0;
}

void current_user_number(int *number, struct User (*user)[MAX_USER])
{
    *number = 0;
    for (int i = 0; i < MAX_USER; i++)
    {
        if (user[i]->free == 1)
            break;
        (*number)++;
    }
}
void logout(int queue_id, struct User (*user)[MAX_USER], int *status)
{
    for (int i = 0; i < MAX_USER; i++)
    {
        if (user[i]->queue_id==queue_id)
        {
            for (int j = i; j < MAX_USER - 1; j++)
            {
                strcpy(user[j]->nick, user[j + 1]->nick);
                user[j]->queue_id = user[j + 1]->queue_id;
                user[j]->free = user[j + 1]->free;
            }
            break;
        }
    }
    // Usunięcie użytkownika i przesunięcie pozostałych
}
