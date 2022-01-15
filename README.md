# PSiW-projekt

## Tutorial
### Instalacja i uruchamianie
Do działania programu potrzebna jest biblioteka ncurses.
Kroki do jej instalacji podane są na stronie: https://www.cyberciti.biz/faq/linux-install-ncurses-library-headers-on-debian-ubuntu-centos-fedora/

Aby dokonać kompilacji wystarczy wykonać skrypt "build.sh".
Pojawią się pliki wykonywalne "psiw_klient.exe" i "psiw_serwer.exe".

### Serwer
Po uruchomieniu serwer przypisuje sobie pierwszy wolny klucz z pliku "config.in" i jest gotowy do działania.
W jego oknie "zmiany" można przeglądać jego pracę.

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
Wykorzystujemy istniejącą kolejkę wejściową serwera i kolejkę wejściową klienta, nie tworzymy nowych. 
Serwer komunikuje się z innymi przy pomocy ich kolejek wejściowych, tzn. tworzy pakiet o konkretnym nagłówku, który ma za zadanie aktualizacje kanałów.

## Wymogi
Max kientów na serwer: 5
Max serwerów: 10

## Klient
* Pyta, który serwer (może być juz pełny)
* Podaj nazwę użytkownika (może być zajęta)
* menu:
    - lista użytkowników na serwerze
    - lista pokoi i użytkowników na serwerze
    - zapisz/wypisz się z pokoju
    - 10 ostatich wiadomości w pokoju
    - wyślij wiadomość do pokoju
    - wyślij wiadomość prywatną
    - wylogowanie się