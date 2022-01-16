# BUILD C PROGRAM WITH REQUIRED LIBRARIES

mkdir -p ./logs;
chmod +x *
gcc ./src/common.c ./inc/common.h ./inc/client/* ./src/client/* ./libncurses.a -o psiw_klient.exe -ldl;
gcc ./src/common.c ./inc/common.h ./inc/server/* ./src/server/* ./libncurses.a -o psiw_serwer.exe -ldl;
# gcc ./inc/client/* ./src/client/* -o psiw_client.exe -lncurses;
