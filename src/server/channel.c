
#include "../../inc/server/channel.h"
#include "../../inc/server/user.h"


void add_new_channel(struct Channel *channels, int *id, struct Mess *mess)
{
    int num = num_of_channels(channels);
    if(num==-1){
        *id = -1;
        return;
    }
    channels[num].id=num+1;
    channels[num].free=0;
    strcpy(channels[num].name,mess->body);
    init_user_channel_struct(channels[num].users,10);
    channels[num].users[0].free=0;
    strcpy(channels[num].users[0].nick,mess->from_client_name);
    channels[num].users[0].queue_id=mess->from_client;
    for (int j = 0; j < 100; j++)
    {
        clear_mess(&channels[num].messages[j]);
    }
    strcpy(channels[num].messages[0].body,mess->body);
    channels[num].messages[0].timestamp=time(NULL);
    channels[num].messages[0].from_client=0;
    strcpy(channels[num].messages[0].from_client_name,"Utworzono nowy kanal");
    *id = num+1;
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

void remove_from_channel_list_id(struct Channel *channels,  int id)
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

int num_of_channels(struct Channel *channels)
{

    for (int i = 0; i < MAX_CHANNEL; i++)
    {
        if (channels[i].free)
            return i;
    }
    return -1;
}


void init_channel_struct(struct Channel *channel)
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

void send_last_ten_msg_from_channel(struct Channel *channel,int channel_id,int user_queue,int current_server_id){
    struct Mess request;
    request.msgid = 11;
    request.from_server=current_server_id;
    request.to_chanel =channel_id;
    for(int i=0;i<MAX_CHANNEL;i++){
        if(channel[i].id==channel_id){
            for(int j=9;j>=0;j--){
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
void send_channel_msg_to_users(struct Channel *channels, struct Mess request){

    for(int i=0;i<MAX_CHANNEL;i++){
        if(channels[i].id==request.to_chanel){
            for(int j=0;j<10;j++){
                if(channels[i].users[j].queue_id!=0){
                    msgsnd(channels[i].users[j].queue_id, &request, sizeof(request) - sizeof(long), 0);
                }
            }
            break;
        }
    }
}