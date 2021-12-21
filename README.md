# PSiW-projekt

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