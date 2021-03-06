#include "../../inc/server/user.h"

void init_user_struct(struct User *user)
{
    for (int i = 0; i < MAX_USER; i++)
    {
        user[i].free = 1;
        user[i].queue_id = 0;
        strcpy(user[i].nick, "");
    }
}
void init_user_channel_struct(struct User *user, int n)
{
    for (int i = 0; i < n; i++)
    {
        user[i].free = 1;
        user[i].queue_id = 0;
        strcpy(user[i].nick, "");
        for (int j = 0; j < 100; j++)
        {
            clear_mess(&user[i].messages[j]);
        }
    }
}

void registration(struct Mess request, int server_queue_id, struct User *user, int *status)
{
    struct Mess response;
    int nr_of_clients;
    *status = 0;
    current_user_number(&nr_of_clients, user);
    if (nr_of_clients > 4)
    {
        *status = -1;
        return;
    }
    // Sprawdzenie czy nazwa już nie występuje
    for (int i = 0; i < nr_of_clients; i++)
    {
        if (!strcmp(user[i].nick, request.from_client_name))
        {
            *status = -1;
        }
    }
    // Dodanie użytkownika
    strcpy(user[nr_of_clients].nick, request.from_client_name);
    user[nr_of_clients].queue_id = request.from_client;
    user[nr_of_clients].server_host = server_queue_id;
    user[nr_of_clients].free = 0;

    response.msgid = 2;
    response.from_server = server_queue_id;
    response.body[0] = *status + 48;
    msgsnd(request.from_client, &response, sizeof(response) - sizeof(long), 0);
}

void current_user_number(int *number, struct User *user)
{
    *number = 0;
    for (int i = 0; i < MAX_USER; i++)
    {
        if (user[i].free == 1)
            break;
        (*number)++;
    }
}
void logout(int queue_id, struct User *user, int *status)
{
    for (int i = 0; i < MAX_USER; i++)
    {
        if (user[i].queue_id == queue_id)
        {
            for (int j = i; j < MAX_USER - 1; j++)
            {
                strcpy(user[j].nick, user[j + 1].nick);
                user[j].queue_id = user[j + 1].queue_id;
                user[j].free = user[j + 1].free;
            }
            if (i = MAX_USER-1){
                strcpy(user[i].nick, "");
                user[i].queue_id = 0;
                user[i].free = 1;
            }
            break;
        }
    }
    // Usunięcie użytkownika i przesunięcie pozostałych
}
