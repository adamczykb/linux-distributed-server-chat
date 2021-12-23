#include "../inc/common.h"

void clear_mess(struct Mess *mess)
{
    mess->msgid = 0;
    mess->from_server = 0;
    mess->from_client = 0;
    strcmp(mess->from_client_name, "");
    mess->to_chanel = 0;
    strcmp(mess->to_client_name, "");
    mess->timestamp = (time_t)NULL;
    strcmp(mess->body, "");
}