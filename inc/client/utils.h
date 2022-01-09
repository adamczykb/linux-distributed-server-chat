#include "../../inc/common.h"
#ifndef PSIW_USER_UTILS_H
#define PSIW_USER_UTILS_H

#define MAX_CHANNEL 50

enum typing_target
{
    TYPING_TARGET_NO_DECLARED=0,
    TYPING_TARGET_NEW_CHANNEL=1,
    TYPING_TARGET_NEW_MESSAGE=2
};

enum message_type
{
    MSG_TYPE_NO_DECLARED=0,
    MSG_TYPE_CHANNEL=1,
    MSG_TYPE_DIRECT=2
};

void init_user_struct(struct User *user, int n);
int num_of_users(struct User *users, int n);
int num_of_channels(struct Channel *channels);
void init_channel_struct(struct Channel *channel);
void new_channel(struct Channel *channels, struct Mess *mess);
void remove_from_channel_list_id(struct Channel *channels, int id);
void add_dm_message(struct Channel *channels, struct Mess *response);
#endif