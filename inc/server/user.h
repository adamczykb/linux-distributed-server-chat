#include <string.h>



#ifndef PSIW_USER_H
#define PSIW_USER_H
#define MAX_USER 5
struct User{
    int queue_id;
    char nick[100];
    int free; // 0 -> nie 1 -> tak
};

void init_user_struct(struct User (*user)[MAX_USER]);
void logout(int queue_id,struct User (*user)[MAX_USER],int *status);
void registration(int queue_id, char nickname[100], struct User (*user)[MAX_USER], int *status);
void current_user_number(int *number, struct User (*user)[MAX_USER]);
#endif