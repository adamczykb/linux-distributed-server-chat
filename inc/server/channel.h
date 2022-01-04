
#include "../common.h"

#ifndef PSIW_CHANNEL_H
#define PSIW_CHANNEL_H

#define MAX_CHANNEL 50

void add_new_channel(struct Channel *channels,  int *id, struct Mess *mess);
void remove_from_channel_list_name(struct Channel *channels,  char *name);
void remove_from_channel_list_id(struct Channel *channels, int id);
int num_of_channels(struct Channel *channels);
void init_channel_struct(struct Channel *channel);

#endif