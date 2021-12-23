#include <string.h>

#ifndef PSIW_USER_H
#define PSIW_USER_H

int logout(char nickname[100], char *current_clients[][100], int *nr_of_clients);
int registration(char nickname[100], char *current_clients[][100], int *nr_of_clients);

#endif