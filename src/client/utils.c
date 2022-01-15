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

int get_server_id(int server_nr)
{
    int server_id = msgget(server_nr, 0644);
    return server_id;
}

void new_channel(struct Channel *channels, struct Mess *mess)
{
    int num = num_of_channels(channels);

    channels[num].id = mess->to_chanel;
    channels[num].free = 0;
    if (channels[num].id == 1)
    {
        channels[num].usr_signed = 1;
    }
    else
    {
        channels[num].usr_signed = 0;
    }
    strcpy(channels[num].name, mess->body);

    for (int j = 0; j < 100; j++)
    {
        clear_mess(&channels[num].messages[j]);
    }

    init_user_struct(channels[num].users, 10);
    // channels[num].users[0].free=0;
    // strcpy(channels[num].users[0].nick,mess->from_client_name);
    // channels[num].users[0].queue_id=mess->from_client;
}

void remove_from_channel_list_id(struct Channel *channels, int id)
{
    for (int i = 0; i < MAX_CHANNEL; i++)
    {
        if (id == channels[i].id)
        {
            for (int j = i; j < MAX_CHANNEL - 1; j++)
            {
                strcpy(channels[j].name, channels[j + 1].name);
                channels[j].id = channels[j + 1].id;
                channels[j].free = channels[j + 1].free;
                memcpy(channels[j].messages, channels[j + 1].messages, sizeof(char) * 100 * 100);
            }
            break;
        }
    }
    // Usunięcie kanalu i przesunięcie pozostałych na liscie
}

void add_dm_message(struct Channel *channels, struct Mess *response)
{
    for (int i = 0; i < MAX_CHANNEL; i++)
    {
        if (response->to_chanel == channels[i].id)
        {
            for (int j = 0; j < 10; j++)
            {
                if (response->to_user == channels[i].users[j].queue_id)
                {

                    for (int k = 98; k >= 0; k--)
                    {
                        channels[i].users[j].messages[k + 1] = channels[i].users[j].messages[k];
                    }
                    strcpy(channels[i].users[j].messages[0].body, response->body);
                    strcpy(channels[i].users[j].messages[0].from_client_name, response->from_client_name);
                    channels[i].users[j].messages[0].timestamp = response->timestamp;
                    channels[i].users[j].messages[0].from_client = response->from_client;
                    channels[i].users[j].messages[0].from_server = response->from_server;
                    break;
                }
            }
        }
    }
}
