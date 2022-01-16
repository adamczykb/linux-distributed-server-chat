# PSiW-projekt

## Tutorial
### Instalacja i uruchamianie
Do działania programu potrzebna jest biblioteka ncurses.
Kroki do jej instalacji podane są na stronie: https://www.cyberciti.biz/faq/linux-install-ncurses-library-headers-on-debian-ubuntu-centos-fedora/

Aby dokonać kompilacji wystarczy wykonać skrypt "build.sh".
Pojawią się pliki wykonywalne "psiw_klient.exe" i "psiw_serwer.exe".

### Serwer
Po uruchomieniu serwer przypisuje sobie pierwszy wolny klucz z pliku "config.in" i jest gotowy do działania.
W jego oknie "Zmiany" można przeglądać jego pracę.

### Klient
Po uruchomieniu klienta należy podać swój nick i wybrać serwer do podłączenia (jeśli serwer nie jest dostępny powinien pojawić się o tym komunikat).

Po zalogowaniu na serwer możemy poruszać się pomiędzy 3 okami strzałkami "lewo" "prawo", natomiast w ramach okien strzałkami "góra" "dół".

Aby odczytywać/wysyłać wiadomości należy być na odpowiednim kanale.
Na początek każdy użytkownik jest podłączony do kanału "Globalny", który zawiera wszystkich użytkowników serwera.
Nowe kanały można utworzyć w oknie kanałów, one mogą posiadać użytkowników z dowolnych serwerów.
Wiadomości prywatne mogą być przesyłane poprzez wybranie odpowiedniego użytkownika w oknie użytkowników.
Uwaga! Każdy kanał będzie miał oddzielne wiadomości prywatne. Czyli jeżeli 2. użytkowników zacznie pisać wiadomości prywatne do siebie, będąc na różnych kanałach, nie zobaczą oni ich. Muszą się połączyć, będąc na tym samym kanale.

Aby wysłać wiadomość wystarczy wpisać ją w oknie wiadomości i nacisnąć "Enter".


## Ustalenia
### Komunikacja klient-serwer
Każdy serwer i każdy klient tworzy swoją kolejkę przy uruchomieniu.
Klienci pobierają numery serwerów z pliku "config.in" dzięki czemu wiedzą do jakich serwerów mogą się podłączyć.
Po połączeniu klient wysyła swoje wiadomości na kolejkę serwera, a serwer odpowiada, wysyłając wiadomość tego samego typu na kolejkę klienta.

### Komunikacja serwer-serwer
Serwery komunikują się pomiędzy sobą przy pomocy ich kolejek wejściowych.
Przy informacjach, które muszą dotrzeć do wszystkich serwerów (np. utworzenie/usunięcie kanałów) serwer, który posiada wiadomość wysyła ją, w niezmienionej formie, do wszystkich pozostałych serwerów.

### Kanały
Każdy serwer ma kanał o nazwie "Globalny", zawierający wszystkich użytkowników serwera.
Kanały utworzone przez użytkowników są widoczne i mogą być używane przez użytkowników wszystkich serwerów.

### Wiadomości prywatne
Wiadomości prywatne są realizowane jako kanały dwuosobowe, przypisane do konkretnego kanału właściwego.
Przykład:
Gdy 2. użytkowników łączy się przez kanał "Globalny" i używa funkcji wiadomości prywatnych tworzony jest kanał prywatny przypisany do kanału "Globalny".
Jeśli połączą się za pomocą innego kanału niż "Globalny" to utworzą inny kanał prywatny.

### Typ komunikacji
Każdy program tworzy swoją własna kolejkę, która działa podobnie do skrzynki pocztowej. Do tych kolejek trafiają wiadomości. Odsyłanie informacji wiąże się ze znajomością adresu serwera, do którego kierujemy odpowiedź, on decyduje gdzie wiadomość pójdzie dalej.

### Kody pakietów
<2> wymiana informacji -> rejestracja użytkownika \
<3> wymiana informacji -> rejestracja kanału oraz zwrot 10 ostatnich wiadomości \
<4> wymiana informacji -> zapisanie do kanału (user) \
<5> wymiana informacji -> wypisanie z kanału (user) \
<6> wymiana informacji -> akceptacja \
<7> wymiana informacji -> podczas logowania serwera wraz z 10 ostatnimi wiadomościami na kanałach \
<8> wymiana informacji -> lista kanałów \
<9> wymiana informacji -> lista użytkowników na kanale \
<11> wiadomość -> z kanału \
<13> wiadomość -> prywatna \
<14> akcja -> wylogowanie (user) \
<15> akcja -> wylogowanie (serwer) \
<16> logowanie -> serwer \
<20> HeartBeat 


### Struktura pakietu
```c
struct Mess
{
    long msgid;                 // identyfikator pakietu
    int from_server;            // deskryptor serwera
    int from_client;            // deskryptor klienta
    char from_client_name[100]; // nick klienta
    int to_chanel;              // id kanalu do ktorego kierowana jest wiadomosc
    int to_user;                // deskryptor klienta do ktorego kierowany jest pakiet
    int broadcasted;            // informacja o rozpowszechnieniu wiadomosci miedzy serwerami
    char to_client_name[100];   // nick adresata
    time_t timestamp;           // czas utworzenia pakietu
    char body[1024];            // obszar dla pozostalych wiadomosci
    int for_server;             // informacja o lokalizacji usera
};
```

## Wymogi
Max kientów na serwer: 5
Max serwerów: 10
