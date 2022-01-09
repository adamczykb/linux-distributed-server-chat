#include "../inc/common.h"

void clear_mess(struct Mess *mess)
{
    mess->msgid = 0;
    mess->from_server = 0;
    mess->from_client = 0;
    strcmp(mess->from_client_name, "");
    mess->to_chanel = 0;
    strcmp(mess->to_client_name, "");
    mess->timestamp = (time_t)NULL;
    strcmp(mess->body, "");
}

void num_of_config_lines(int *nr_of_lines, char *path_to_config)
{
    int file = open(path_to_config, O_RDWR);
    char buff[100] = "";
    for (int i = 0; read(file, &buff[i], 1) > 0; i++)
        if (buff[i - 1] == '\n' && buff[i] >= 48 && buff[i] <= 57)
            *nr_of_lines += 1;
    close(file);
}

void read_config(int *servers_keys, char *path_to_config) // czytamy config do listy dostepnych kluczy serwera
{
    int file = open(path_to_config, O_RDWR);
    char buff[100] = "";
    int nr_of_lines = 1;
    for (int i = 0; read(file, &buff[i], 1) > 0; i++)
        if (buff[i - 1] == '\n' && buff[i] >= 48 && buff[i] <= 57)
            nr_of_lines += 1;
    close(file);

    int current_v = 0, current_i = 1;
    servers_keys[0] = nr_of_lines;
    for (int i = 0; i < 100; i++)
    {
        if (buff[i] == '\0')
            break;
        if (buff[i] == '\n')
        {
            servers_keys[current_i] = current_v;
            current_v = 0;
            current_i += 1;
            continue;
        }
        current_v = current_v * 10 + buff[i] - 48;
    }
    if (current_v != 0)
        servers_keys[current_i] = current_v;
}


void boxDescription(WINDOW *pwin, const char *title)
{
    int x, stringsize;
    stringsize = 4 + strlen(title);
    x = 1;
    mvwaddch(pwin, 0, x, ' '); //mv - move, w - window, add - dodaj, ch - char
    waddstr(pwin, title);
    waddch(pwin, ' ');
}

void add_user_to_channel(struct Channel *channels, struct Mess *mess,int *result){
    *result=-1;
    for(int i =0;i<MAX_CHANNEL;i++){
        if(channels[i].id==mess->to_chanel){
            for(int j=0;j<10;j++){
                if(channels[i].users[j].free==1){
                    channels[i].users[j].free=0;
                    strcpy(channels[i].users[j].nick,mess->from_client_name);
                    channels[i].users[j].queue_id=mess->from_client;
                    *result=0;
                    break;
                }
            }
            break;
        }
    }
}
void remove_user_from_channel(struct Channel *channels, struct Mess *mess,int *result){
    *result=-1;
    for(int i =0;i<MAX_CHANNEL;i++){
        if(channels[i].id==mess->to_chanel){
            for(int j=0;j<10;j++){
                if(channels[i].users[j].queue_id==mess->from_client){
                    for(int k=j;k<9;k++){
                        channels[i].users[k].free=channels[i].users[k+1].free;
                        strcpy(channels[i].users[k].nick,channels[i].users[k+1].nick);
                        channels[i].users[k].queue_id=channels[i].users[k+1].queue_id;
                    }
                    *result=0;
                    break;
                }
            }
            break;
        }
    }
}

void add_msg_to_channel(struct Channel *channels, struct Mess *mess){
    int i;
    for(i=0;i<MAX_CHANNEL;i++){
        if(channels[i].id==mess->to_chanel){
            for(int j =98;j>=0;j--){
                channels[i].messages[j+1]=channels[i].messages[j];
            }
            strcpy(channels[i].messages[0].body,mess->body);
            strcpy(channels[i].messages[0].from_client_name,mess->from_client_name);
            channels[i].messages[0].timestamp=mess->timestamp;
            channels[i].messages[0].from_client=mess->from_client;
            channels[i].messages[0].from_server=mess->from_server;
            break;
        }
    }
}