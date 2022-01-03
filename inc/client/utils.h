#include "../../inc/common.h"
#ifndef PSIW_USER_UTILS_H
#define PSIW_USER_UTILS_H
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

struct User
{
    int queue_id;
    char nick[100];
    struct Mess messages[100];
    int free; // 0 -> nie 1 -> tak
};

struct Channel
{
    int queue_id;
    char name[100];
    struct User users[10];
    struct Mess messages[100]; //wiem ze mialo byc 10 ale mysle ze 10 ma przysylac serwer
    int free;                  // 0 -> nie 1 -> tak
};
void init_user_struct(struct User *user, int n);
int num_of_users(struct User *users, int n);
int num_of_channels(struct Channel *channels, int n);
void init_channel_struct(struct Channel *channel, int n);
#endif