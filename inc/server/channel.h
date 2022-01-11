
#include "../common.h"

#ifndef PSIW_CHANNEL_H
#define PSIW_CHANNEL_H

void add_new_channel(struct Channel *channels, int *id, int *channel_index, struct Mess *mess);
void remove_from_channel_list_name(struct Channel *channels, char *name);
void remove_from_channel_list_id(struct Channel *channels, int id);
int num_of_channels(struct Channel *channels);
void init_channel_struct(struct Channel *channel);
void send_last_ten_msg_from_channel(struct Channel *channel, int channel_id, int user_queue, int current_server_id);
void send_channel_msg_to_users(struct Channel *channels, struct Mess request,int current_server_id);
void channel_info_on_user_login(struct Channel *channels, struct Mess *request, int from_server);
void send_created_channel(int to, struct Channel channel, int from_server);
void send_new_channel_member(int to, int channel_id, int creator_id,char *creator_nick, int from_server);
void add_user_to_channel_server_response(struct Channel *channels,struct Mess *request, struct User *user, int current_server_id,int result);
void remove_user_from_channel_server_response(struct Channel *channels, struct Mess *request, struct User *user, int current_server_id, int result);
void send_removed_channel(int to, int channel_id, int from_server);
void channel_info_on_server_login(struct Channel *channels, struct Mess *request, int from_server);
#endif