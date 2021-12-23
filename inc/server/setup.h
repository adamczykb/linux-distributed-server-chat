#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef PSIW_SETUP_H
#define PSIW_SETUP_H

void load_config(int *server_key, int *servers_ids, char *path_to_config);

#endif