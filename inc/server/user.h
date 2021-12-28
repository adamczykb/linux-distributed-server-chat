#include <string.h>

#ifndef PSIW_USER_H
#define PSIW_USER_H

void logout(char nickname[100], char *current_clients[][100], int *nr_of_clients,int *status);
void registration(char nickname[100], char *current_clients[][100], int *nr_of_clients,int *status);

#endif