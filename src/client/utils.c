#include "../../inc/client/utils.h"

void init_user_struct(struct User *user, int n)
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

int num_of_users(struct User *users, int n)
{
    int i = 0;
    for (i = 0; i < n; i++)
    {
        if (users[i].free)
            break;
    }
    return i;
}

int num_of_channels(struct Channel *channels, int n)
{
    int i = 0;
    for (i = 0; i < n; i++)
    {
        if (channels[i].free)
            break;
    }
    return i;
}

void init_channel_struct(struct Channel *channel, int n)
{
    for (int i = 0; i < n; i++)
    {
        channel[i].free = 1;
        channel[i].queue_id = 0;
        strcpy(channel[i].name, "");
        init_user_struct(channel->users, 10);
        for (int j = 0; j < 100; j++)
        {
            clear_mess(&channel[i].messages[j]);
        }
    }
    channel[0].free = 0;
    channel[0].queue_id = 0;
    strcpy(channel[0].name, "LOL");
    strcpy(channel[0].users[0].nick, "Jan");
    channel[0].users[0].free = 0;
}