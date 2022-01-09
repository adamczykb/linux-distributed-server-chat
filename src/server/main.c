
#include "../../inc/common.h"
#include "../../inc/server/ui.h"
#include "../../inc/server/setup.h"
#include "../../inc/server/user.h"
#include "../../inc/server/logger.h"
#include "../../inc/server/channel.h"
#include "../../inc/server/trans.h"

WINDOW *info_window;
WINDOW *server_banner_window;
WINDOW *log_window;
WINDOW *background;

struct Log *log;
struct Channel *channels;
struct User *user;

int result;

int current_server_id = 0;
time_t current_time;
int server_key = 0;
int *servers_ids;
int userSID, logSID, channelSID;
int log_descriptor;
// nastepuje tu pobranie kluczy kolejek, zostalo to rozbite na dwa bo potrzebujemy listy pozostalych serwerow a nie mozna przeslac jako parametr nieokreslonego pointera

void heartbeat_starter();

void setup(char *config_path)
{
    int nr_of_lines = 1, channel_id, channel_index;
    num_of_config_lines(&nr_of_lines, config_path);
    servers_ids = malloc(sizeof(int) * nr_of_lines + 1);
    load_config(&server_key, servers_ids, config_path);
    current_server_id = msgget(server_key, 0644 | IPC_CREAT);
    // channels = malloc(sizeof(struct Channel) * MAX_CHANNEL);

    if (current_server_id == -1)
    {
        perror(TR_QUEUE_ERROR);
        exit(1);
    }
    heartbeat_starter();

}
void end_of_work()
{
    msgctl(current_server_id, IPC_RMID, NULL);
    shmdt(user);
    shmdt(log);
    shmctl(userSID, IPC_RMID, NULL);
    shmctl(logSID, IPC_RMID, NULL);
    printf(TR_SERVER_STOPPED);
    endwin();
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, end_of_work);

    setup("config.in");

    // tworzenie pliku logów
    char log_pathname[30];
    sprintf(log_pathname, "./logs/%d_serwer.log", server_key);
    log_descriptor = open(log_pathname, O_CREAT | O_APPEND | O_WRONLY, 0644);
    //===========

    // cykl życia serwera
    struct Mess request, response;
    int status;
    clear_mess(&request);
    clear_mess(&response);
    initscr();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    int reg_outcome;
    char foo[2048];

    while (1)
    {

        window_init(user, log, server_key, background, server_banner_window, info_window, log_window);

        status = msgrcv(current_server_id, &request, sizeof(request) - sizeof(long), -17, IPC_NOWAIT);
        if (status == -1)
        {
            usleep(100 * 1000);
            continue;
        }
        int usr_exist = -1;
        for (int i = 0; i < MAX_USER; i++)
        {
            if (user[i].queue_id == request.from_client)
            {
                usr_exist = 0;
                break;
            }
        }
        if (usr_exist == -1 && request.msgid != 2)
            continue;
        current_time = time(NULL);
        switch (request.msgid)
        {
        case 0:
            break;
        case 2: // rejestracja użytkownika
            registration(request, current_server_id, user, &reg_outcome);

            if (reg_outcome == 0)
            {
                char from_client_string[20];
                sprintf(from_client_string, "%d", request.from_client);
                add_to_log(log, time(NULL), TR_USER_JOINED, from_client_string, request.from_client_name, 0);

                request.to_chanel = 1;
                add_user_to_channel(channels, &request, &result);
                channel_info_on_user_login(channels, &request, current_server_id);
                send_last_ten_msg_from_channel(channels, 1, request.from_client, current_server_id);

                response.msgid = 4;
                response.to_chanel = 1;
                response.from_client = request.from_client;
                strcpy(response.from_client_name, request.from_client_name);
                for (int i = 0; i < MAX_USER; i++)
                {
                    if (user[i].queue_id != 0 && user[i].queue_id != request.from_client)
                        msgsnd(user[i].queue_id, &response, sizeof(response) - sizeof(long), 0);
                }
            }

            break;
        case 3: // nowy kanal
            int channel_id, channel_index;
            add_new_channel(channels, &channel_id, &channel_index, &request);
            if (channel_id != -1)
            {
                for (int i = 0; i < MAX_USER; i++)
                {
                    if (user[i].free == 0)
                    {
                        send_created_channel(user[i].queue_id, channels[channel_index], current_server_id);
                        send_new_channel_member(user[i].queue_id, channel_id, request.from_client, request.from_client_name, current_server_id);
                    }
                }

                send_last_ten_msg_from_channel(channels, channel_id, request.from_client, current_server_id);

                add_to_log(log, time(NULL), "Pomyslnie utworzono kanal", "localhost", request.body, 0);
            }
            else
                add_to_log(log, time(NULL), "Blad podczas tworzenia kanalu", "localhost", request.body, 1);
            break;
        case 4:
            add_user_to_channel(channels, &request, &result);
            add_user_to_channel_server_response(channels, &request, user, current_server_id, result);

            break;
        case 5:
            remove_user_from_channel(channels, &request, &result);
            remove_user_from_channel_server_response(channels, &request, user, current_server_id, result);
            
            int user_num, srv_index = -1;
            for (int i = 1; i < MAX_CHANNEL; i++)
            {
                 if(channels[i].free==1)
                break;
                if (channels[i].id == request.to_chanel)
                {
                    srv_index = i;
                    break;
                }
               
            }
            if (srv_index > 0)
            {
                current_user_number(&user_num, channels[srv_index].users);
                if (user_num == 0)
                {
                    for (int i = 0; i < MAX_USER; i++)
                    {
                        if (user[i].free == 0)
                            send_removed_channel(user[i].queue_id, request.to_chanel, current_server_id);
                    }
                    remove_from_channel_list_id(channels, channels[srv_index].id);
                }
            }
            break;
        case 11:
            add_msg_to_channel(channels, &request);
            send_channel_msg_to_users(channels, request);
            break;
        default: // obsluga blednego pakietu
            sprintf(foo, "\n%s\nBlad pakietu %ld\nBody:%s\nFrom client:%d\nCHANNEL: %d\n-----------\n", ctime(&current_time), request.msgid, request.body, request.from_server, request.to_chanel);
            write(log_descriptor, foo, strlen(foo));
            sprintf(foo, "\n%s\nBlad pakietu %ld\nBody:%s\nFrom client:%d\nCHANNEL: %d\n-----------\n", ctime(&current_time), response.msgid, response.body, response.from_server, request.to_chanel);
            write(log_descriptor, foo, strlen(foo));
            sleep(1);
            break;
        }
        clear_mess(&request);
        clear_mess(&response);
    }

    msgctl(current_server_id, IPC_RMID, NULL);
    return 0;
}

void heartbeat_starter()
{
    struct Mess request, response;
    clear_mess(&request);
    clear_mess(&response);
    srand(time(0));
    int shmget_log = rand() % 1000000;
    int shmget_user = rand() % 1000000;
    int shmget_channel = rand() % 1000000;

    if (fork() == 0)
    {
        if ((userSID = shmget(shmget_user, sizeof(struct User) * MAX_USER, IPC_CREAT | 0644)) == -1)
        {
            perror("shmget");
            exit(1);
        }
        if ((logSID = shmget(shmget_log, sizeof(struct Log) * MAX_LOG, IPC_CREAT | 0644)) == -1)
        {
            perror("shmget");
            exit(1);
        }
        if ((channelSID = shmget(shmget_channel, sizeof(struct Channel) * MAX_CHANNEL, IPC_CREAT | 0644)) == -1)
        {
            perror("shmget");
            exit(1);
        }
        user = (struct User *)shmat(userSID, 0, 0);
        log = (struct Log *)shmat(logSID, 0, 0);
        channels = (struct Channel *)shmat(channelSID, 0, 0);

        init_log(log);
        init_channel_struct(channels);

        struct Mess request;
        strcpy(request.body, "Globalny");
        request.from_client = 0;
        int  channel_id, channel_index;
        add_new_channel(channels, &channel_id, &channel_index, &request);

        char server_id_string[255];
        sprintf(server_id_string, "%d", current_server_id);
        add_to_log(log, time(NULL), TR_SERVER_STARTED, server_id_string, TR_READY, 0);

        int client_number, status;
        sleep(1);
        while (getppid() != 1)
        {
            current_user_number(&client_number, user);
            for (int i = 0; i < client_number; i++)
            {
                request.msgid = 20;
                request.from_server = current_server_id;
                request.body[0] = 'A';
                msgsnd(user[i].queue_id, &request, sizeof(request) - sizeof(long), 0);
                sleep(1);
                status = msgrcv(current_server_id, &response, sizeof(response) - sizeof(long), 20, IPC_NOWAIT);
                if (status == -1 || request.body[0] != response.body[0])
                {
                    char from_client_string[20];
                    sprintf(from_client_string, "%d", user[i].queue_id);
                    if (strlen(user[i].nick) > 0)
                        add_to_log(log, time(NULL), TR_USER_LEFT, from_client_string, user[i].nick, 0);

                    clear_mess(&request);
                    clear_mess(&response);
                    int j = 0;
                    for (j = 0; j < MAX_CHANNEL; j++)
                    {
                        if (channels[j].free == 1)
                            break;
                        request.from_client = user[i].queue_id;
                        request.to_chanel = channels[j].id;
                        remove_user_from_channel(channels, &request, &result);
                        remove_user_from_channel_server_response(channels, &request, user, current_server_id, result);
                        int user_num;
                        current_user_number(&user_num, channels[j].users);
                        if (user_num < 1 && j!=0)
                        {
                            send_removed_channel(user[i].queue_id, request.to_chanel, current_server_id);
                            remove_from_channel_list_id(channels, channels[j].id);
                            j--;
                        }
                    }
                    logout(user[i].queue_id, user, &status);
                }
                clear_mess(&request);
                clear_mess(&response);
            }
            sleep(1);
        }
        exit(1);
    }

    if ((userSID = shmget(shmget_user, sizeof(struct User) * MAX_USER, IPC_CREAT | 0644)) == -1)
    {
        perror("shmget");
        exit(1);
    }

    if ((logSID = shmget(shmget_log, sizeof(struct Log) * 500, IPC_CREAT | 0644)) == -1)
    {
        perror("shmget");
        exit(1);
    }
    if ((channelSID = shmget(shmget_channel, sizeof(struct Channel) * MAX_CHANNEL, IPC_CREAT | 0644)) == -1)
    {
        perror("shmget");
        exit(1);
    }
    user = (struct User *)shmat(userSID, 0, 0);
    log = shmat(logSID, 0, 0);
    channels = (struct Channel *)shmat(channelSID, 0, 0);

    init_user_struct(user);
}