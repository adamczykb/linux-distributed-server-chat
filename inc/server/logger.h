#include <time.h>
#include <string.h>

#ifndef PSIW_LOGGER_H
#define PSIW_LOGGER_H

#define MAX_LOG 500

struct Log
{
    time_t time;
    char head[50];
    char from[100];
    char body[100];
    int error; // 0 -> false 1 -> true
    int empty; // 0 -> false 1 -> true
};

void init_log(struct Log *log);
void add_to_log(struct Log *log, time_t time, char *head, char *from, char *body,int error);
#endif