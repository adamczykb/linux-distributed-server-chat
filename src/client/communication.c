#include "../../inc/client/communication.h"
#include "../../inc/common.h"

void connect_to_server(int server_nr, char nick[100], int client_queue_id, int *result)
{
    int server_id = msgget(server_nr, 0644);
    if (server_id == -1)
    {
        *result = -1;
        return;
    }

    struct Mess registration_msg;
    clear_mess(&registration_msg);

    registration_msg.msgid = 2;
    registration_msg.from_client = client_queue_id;

    strcpy(registration_msg.from_client_name, nick);
    msgsnd(server_id, &registration_msg, sizeof(registration_msg) - sizeof(long), 0);

    struct Mess registration_ans;
    clear_mess(&registration_ans);
    msgrcv(client_queue_id, &registration_ans, sizeof(registration_ans) - sizeof(long), 2, 0);
    if (registration_ans.body[0] == '0')
        *result = registration_ans.from_server;
    else
        *result = -1;
}