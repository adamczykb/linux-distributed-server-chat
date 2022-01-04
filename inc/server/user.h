
#include "../common.h"

#ifndef PSIW_USER_H
#define PSIW_USER_H
#define MAX_USER 5


void init_user_struct(struct User *user);
void init_user_channel_struct(struct User *user, int n);
void logout(int queue_id,struct User *user,int *status);
void registration(int queue_id, char nickname[100], struct User *user, int *status);
void current_user_number(int *number, struct User *user);
#endif