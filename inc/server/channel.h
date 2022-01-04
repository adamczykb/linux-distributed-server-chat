
#include "../common.h"

#ifndef PSIW_CHANNEL_H
#define PSIW_CHANNEL_H


void add_new_channel(struct Channel *channels,  int *id, struct Mess *mess);
void remove_from_channel_list_name(struct Channel *channels,  char *name);
void remove_from_channel_list_id(struct Channel *channels, int id);
int num_of_channels(struct Channel *channels);
void init_channel_struct(struct Channel *channel);
void send_last_ten_msg_from_channel(struct Channel *channel,int channel_id,int user_queue,int current_server_id);
void send_channel_msg_to_users(struct Channel *channels, struct Mess request);
#endif