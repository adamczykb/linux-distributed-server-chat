
#include "../../inc/server/channel.h"
#include "../../inc/server/user.h"

void add_new_channel(struct Channel *channels, int *id, int *channel_index, struct Mess *mess) // tworzenie nowego kanalu
{
    int num = num_of_channels(channels);
    if (num == -1)
    {
        *id = -1;
        return;
    }
    channels[num].id = num + 1;
    channels[num].free = 0;
    strcpy(channels[num].name, mess->body);
    init_user_channel_struct(channels[num].users, 10);
    channels[num].users[0].free = 0;
    strcpy(channels[num].users[0].nick, mess->from_client_name);
    channels[num].users[0].queue_id = mess->from_client;
    for (int j = 0; j < 100; j++)
    {
        clear_mess(&channels[num].messages[j]);
    }
    strcpy(channels[num].messages[0].body, mess->body);
    channels[num].messages[0].timestamp = time(NULL);
    channels[num].messages[0].from_client = 0;
    strcpy(channels[num].messages[0].from_client_name, "Utworzono nowy kanal");
    *id = num + 1;
    *channel_index = num;
}

void remove_from_channel_list_name(struct Channel *channels, char *name)
{
    for (int i = 0; i < MAX_CHANNEL; i++)
    {
        if (strcmp(channels[i].name, name) == 0)
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

int num_of_channels(struct Channel *channels) // wysylanie infomacji o ilosci kanalow
{

    for (int i = 0; i < MAX_CHANNEL; i++)
    {
        if (channels[i].free)
            return i;
    }
    return -1;
}

void init_channel_struct(struct Channel *channel) // tworzenie poczatkwoej struktury kanalu
{
    for (int i = 0; i < MAX_CHANNEL; i++)
    {
        channel[i].free = 1;
        channel[i].id = 0;
        strcpy(channel[i].name, "");
        init_user_channel_struct(channel->users, 10);
        for (int j = 0; j < 100; j++)
        {
            clear_mess(&channel[i].messages[j]);
        }
    }
}

void send_last_ten_msg_from_channel(struct Channel *channel, int channel_id, int user_queue, int current_server_id) // wysylanie do podanego uzytkownka 10 ostatnich wiadomosci z kanalu
{
    struct Mess request;
    request.msgid = 11;
    request.from_server = current_server_id;
    request.to_chanel = channel_id;
    for (int i = 0; i < MAX_CHANNEL; i++)
    {
        if (channel[i].id == channel_id)
        {
            for (int j = 9; j >= 0; j--)
            {
                request.from_client = channel[i].messages[j].from_client;
                request.timestamp = channel[i].messages[j].timestamp;
                strcpy(request.from_client_name, channel[i].messages[j].from_client_name);
                strcpy(request.body, channel[i].messages[j].body);
                msgsnd(user_queue, &request, sizeof(request) - sizeof(long), 0);
            }
            return;
        }
    }
}
void send_channel_msg_to_users(struct Channel *channels, struct Mess request) // wysylanie wiadomosci w sposob broadcast do pozostalych czlonkow kanalu
{

    for (int i = 0; i < num_of_channels(channels); i++)
    {
        if (channels[i].id == request.to_chanel)
        {
            for (int j = 0; j < 10; j++)
            {
                if (channels[i].users[j].queue_id != 0)
                {
                    msgsnd(channels[i].users[j].queue_id, &request, sizeof(request) - sizeof(long), 0);
                }
            }
            break;
        }
    }
}

void channel_info_on_user_login(struct Channel *channels, struct Mess *request, int from_server) // wysylanie informacjo o kanalach dla nowologujacego sie uzytkownika
{
    struct Mess response;
    for (int i = 0; i < num_of_channels(channels); i++)
    {
        if (channels[i].id != 0)
        {
            send_created_channel(request->from_client, channels[i], from_server);

            for (int j = 0; j < 10; j++)
            {
                if (channels[i].users[j].free == 0)
                {
                    send_new_channel_member(request->from_client, channels[i].id, channels[i].users[j].queue_id,channels[i].users[j].nick, from_server);
                }
                else
                {
                    break;
                }
            }
        }
    }
}

void send_created_channel(int to, struct Channel channel, int from_server)
{
    struct Mess response;
    response.msgid = 3;
    response.timestamp = time(NULL);
    response.to_chanel = channel.id;
    strcpy(response.body, channel.name);
    msgsnd(to, &response, sizeof(response) - sizeof(long), 0);
}

void send_new_channel_member(int to, int channel_id, int creator_id,char *creator_nick, int from_server)
{
    struct Mess response;
    response.msgid = 4;
    response.to_chanel = channel_id;
    response.from_client = creator_id;
    strcpy(response.from_client_name, creator_nick);
    msgsnd(to, &response, sizeof(response) - sizeof(long), 0);
}

void add_user_to_channel_server_response(struct Channel *channels, struct Mess *request, struct User *user, int current_server_id, int result)
{
    struct Mess response;
    response.msgid = 6;
    response.timestamp = time(NULL);
    response.from_server = current_server_id;
    response.from_client = request->from_client;
    response.to_chanel = request->to_chanel;
    strcpy(response.from_client_name, request->from_client_name);

    if (result == 0)
    {
        response.body[0] = '0';
        msgsnd(request->from_client, &response, sizeof(response) - sizeof(long), 0);
        response.msgid = 4;
        for (int i = 0; i < MAX_USER; i++)
        {
            if (user[i].queue_id != 0)
                msgsnd(user[i].queue_id, &response, sizeof(response) - sizeof(long), 0);
        }

        send_last_ten_msg_from_channel(channels, request->to_chanel, request->from_client, current_server_id);
    }
    else
    {
        response.body[0] = '-';
        msgsnd(request->from_client, &response, sizeof(response) - sizeof(long), 0);
    }
}

void remove_user_from_channel_server_response(struct Channel *channels, struct Mess *request, struct User *user, int current_server_id, int result)
{
    struct Mess response;
    response.msgid = 5;
    response.timestamp = time(NULL);
    response.from_server = current_server_id;
    response.from_client = request->from_client;
    response.to_chanel = request->to_chanel;

    if (result == 0)
    {
        for (int i = 0; i < MAX_USER; i++)
        {
            if (user[i].queue_id != 0)
                msgsnd(user[i].queue_id, &response, sizeof(response) - sizeof(long), 0);
        }
    }
}