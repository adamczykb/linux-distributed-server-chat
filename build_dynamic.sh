mkdir -p ./logs;
chmod +x *;
gcc ./src/common.c ./inc/common.h ./inc/server/* ./src/server/* -o psiw_serwer.exe -lncurses;
gcc ./src/common.c ./inc/common.h ./inc/client/* ./src/client/* -o psiw_klient.exe -lncurses;
