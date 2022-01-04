#include "../../inc/server/logger.h"

void init_log(struct Log *log)
{
    for (int i = 0; i < MAX_LOG; i++)
    {
        strcpy(log[i].body, "");
        strcpy(log[i].from, "");
        strcpy(log[i].head, "");
        log[i].time = (time_t)NULL;
        log[i].empty = 1;
        log[i].error = 0;
    }
}
void add_to_log(struct Log *log, time_t time, char *head, char *from, char *body,int error)
{
    for (int i = MAX_LOG - 1; i > 0; i--)
    {
        log[i].time = log[i - 1].time;
        log[i].empty = log[i - 1].empty;
        log[i].error = log[i - 1].error;
        strcpy(log[i].from, log[i - 1].from);
        strcpy(log[i].head, log[i - 1].head);
        strcpy(log[i].body, log[i - 1].body);
    }

    log[0].empty = 0;
    log[0].time = time;
    log[0].error = error;
    strcpy(log[0].from, from);
    strcpy(log[0].head, head);
    strcpy(log[0].body, body);
}