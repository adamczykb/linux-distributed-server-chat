# BUILD C PROGRAM WITH REQUIRED LIBRARIES
gcc ./src/server/* -o psiw_serwer.exe -lncurses;
gcc ./src/client/* -o psiw_klient.exe -lncurses;
# gcc ./inc/server/* ./src/server/* -o psiw_serwer.exe -lncurses;  
# gcc ./inc/client/* ./src/client/* -o psiw_client.exe -lncurses;