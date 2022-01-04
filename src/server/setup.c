#include "../../inc/server/setup.h"

// Zwraca wskaźnik na tablicę [ilość_serverów, id_kolejki_servera1, ...]
void load_config(int *server_key, int *servers_ids, char *path_to_config)
{
    // odczytanie pliku
    int file = open(path_to_config, O_RDWR);
    char buff[100] = "";
    int nr_of_lines = 1;
    for (int i = 0; read(file, &buff[i], 1) > 0; i++)
        if (buff[i - 1] == '\n' && buff[i] >= 48 && buff[i] <= 57)
            nr_of_lines += 1;
    close(file);
    // zapisanie elementów z pliku do listy

    servers_ids[0] = nr_of_lines;
    int current_v = 0, current_i = 1;

    /*
        sekcja odpowiedzialna za otwarcie kolejek, ktore sa juz aktywne, automatyzujemy wybor klucza dla serwera i przypisujemy go.
        Otwieramy polaczenie z pozostalymi serwerami.
        Ten mechanizm pozwoli nam na automatyczne przyznanie klucza serwera
    */

    int fd[2];
    pipe(fd);
    if (fork() == 0) //pobieranie listy kluczy otwartych w systemie
    {
        close(fd[0]);
        dup2(fd[1], 1);
        char *args[2] = {"./ipcs_script.sh", NULL};
        execv(args[0], args);
    }
    close(fd[1]);
    wait(NULL);
    char bufor[1024];
    int nr_of_ipcs_lines = 0;
    for (int i = 0; read(fd[0], &bufor[i], 1) > 0; i++)
        if (bufor[i - 1] == '\n' && bufor[i] >= 48 && bufor[i] <= 57) //sprawdzanie ile ich jest
            nr_of_ipcs_lines += 1;

    int *opened_queue_keys = (int *)malloc(sizeof(int) * nr_of_ipcs_lines + 1);
    int current_v_ipcs = 0, current_i_ipcs = 0;
    for (int i = 0; i < 1024; i++)
    {
        if (bufor[i] == '\0')
            break;
        if (bufor[i] == '\n')
        {
            opened_queue_keys[current_i_ipcs] = current_v_ipcs; // pobieramy istniejace i zapisujemy
            current_v_ipcs = 0;
            current_i_ipcs += 1;
            continue;
        }
        current_v_ipcs = current_v_ipcs * 10 + bufor[i] - 48;
    }
    if (current_v_ipcs != 0)
        opened_queue_keys[current_i_ipcs] = current_v_ipcs;

    int exist = 0;
    for (int i = 0; i < 100; i++)
    {
        if (buff[i] == '\0')
            break;
        if (buff[i] == '\n')
        {
            exist = 0;
            for (int j = 0; j < nr_of_ipcs_lines + 1; j++)
            {
                if (opened_queue_keys[j] == current_v) //patrzymy czy klucz jest juz zajety
                {
                    exist = 1;
                    break;
                }
            }
            if (exist)
            {
                servers_ids[current_i] = msgget(current_v, 0644 | IPC_CREAT); // otwieramy z nim polaczenie
                current_v = 0;
                current_i += 1;
            }
            else
            {
                // queue_ids[current_i] = msgget(current_v, 0644 | IPC_CREAT); // to bedzie nasz klucz kolejki, nie otwieramy go tu, robimy to w main.c
                *server_key = current_v;
                break;
            }
            continue;
        }
        current_v = current_v * 10 + buff[i] - 48;
    }
    if (current_v != 0)
        servers_ids[current_i] = msgget(current_v, 0644 | IPC_CREAT);

    if (*server_key == 0) //jezel brak wolnych miejsc to wyrzucamy komunikat
    {
        printf(TR_SERVER_NO_KEYS);
        exit(-1);
    }
}

void new_channel(struct Mess *request,struct Channel *channels){
    
}