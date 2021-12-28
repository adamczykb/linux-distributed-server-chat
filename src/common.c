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

void num_of_config_lines(int *nr_of_lines, char *path_to_config){
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
    int nr_of_lines=1;
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

void init_log(struct Log *log){
    for(int i=0;i<500;i++){
        strcpy(log[i].body,"");
        strcpy(log[i].from,"");
        strcpy(log[i].head,"");
        log[i].time = (time_t)NULL;
        log[i].empty=1;
    }
}
void add_to_log(struct Log *log,time_t time,char *head,char *from,char *body){
    for (int i=499;i>0;i--){
        log[i].time=log[i-1].time;
        log[i].empty = log[i-1].empty;
        strcpy(log[i].from,log[i-1].from);
        strcpy(log[i].head,log[i-1].head);
        strcpy(log[i].body,log[i-1].body);
    }

    log[0].empty = 0;
    log[0].time = time;
    strcpy(log[0].from,from);
    strcpy(log[0].head,head);
    strcpy(log[0].body,body);

}