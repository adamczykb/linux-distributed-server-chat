#include "../../inc/common.h"
#include "../../inc/client/communication.h"
#include "../../inc/client/utils.h"

void connect_to_server(int server_nr, char nick[100], int client_queue_id, int *result)
{
    int server_id = get_server_id(server_nr);
    *result = server_id;
    if (server_id == -1) return;

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
    else if (registration_ans.body[0] == '1') // serwer jest pełny
        *result = -1;
    else
        *result = -1;
}


void heartbeat(int client_queue_id,  char* nick, int server_queue_id){
    struct Mess response;
    struct Mess request;
    while (getppid() != 1)
        {
            msgrcv(client_queue_id, &request, sizeof(request) - sizeof(long), 20, 0);
            strcpy(response.body, request.body);
            response.msgid = 20;
            response.from_client = client_queue_id;
            strcpy(response.from_client_name, nick);
            msgsnd(server_queue_id, &response, sizeof(response) - sizeof(long), 0);
        }
    exit(0);
}
