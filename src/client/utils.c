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

int num_of_channels(struct Channel *channels)
{
    int i = 0;
    for (i = 0; i < MAX_CHANNEL; i++)
    {
        if (channels[i].free)
            break;
    }
    return i;
}

void init_channel_struct(struct Channel *channel)
{
    for (int i = 0; i < MAX_CHANNEL; i++)
    {
        channel[i].free = 1;
        channel[i].id = 0;
        strcpy(channel[i].name, "");
        init_user_struct(channel->users, 10);
        for (int j = 0; j < 100; j++)
        {
            clear_mess(&channel[i].messages[j]);
        }
    }
}

int get_server_id(int server_nr, int *result){
    int server_id = msgget(server_nr, 0644);
    if (server_id == -1)
    {
        *result = -1;
        return;
    }
}


void new_channel(struct Channel *channels, struct Mess *mess)
{
    int num = num_of_channels(channels);
    
    channels[num].id=mess->to_chanel;
    channels[num].free=0;
    channels[num].usr_signed=0;
    strcpy(channels[num].name,mess->body);
    
    for (int j = 0; j < 100; j++)
    {
        clear_mess(&channels[num].messages[j]);
    }

    init_user_struct(channels[num].users,10);
    // channels[num].users[0].free=0;
    // strcpy(channels[num].users[0].nick,mess->from_client_name);
    // channels[num].users[0].queue_id=mess->from_client;
}

