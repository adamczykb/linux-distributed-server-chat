struct Channel{
    int queue_id;
    char name[100];
    char messages[100][100]; //wiem ze mialo byc 10 ale mysle ze 10 ma przysylac serwer
    int free; // 0 -> nie 1 -> tak
};
void add_to_channel_list(struct Channel *channels,int n, int queue_id, char *name)
{
    channels[0].free = 0;
    channels[0].queue_id = queue_id;
    strcpy(channels[0].name, name);
    
}

void remove_from_channel_list(struct Channel *channels,int n, char *name)
{
     for (int i = 0; i < n; i++)
    {
        if (strcmp(channels[i].name,name)==0)
        {
            for (int j = i; j < n - 1; j++)
            {
                strcpy(channels[j].name, channels[j + 1].name);
                channels[j].queue_id = channels[j + 1].queue_id;
                channels[j].free = channels[j + 1].free;
                memcpy(channels[j].messages, channels[j + 1].messages, sizeof (char) * 100 * 100);
            }
            break;
        }
    }
    // Usunięcie kanalu i przesunięcie pozostałych na liscie
    
}