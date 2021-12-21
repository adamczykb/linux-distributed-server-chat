# PSiW-projekt

## Ustalenia
Wykorzystujemy istniejacą kolejke wejściową serwera i kolejke wejsciową klienta, nie tworzymy nowych. 
Serwer komunikuje się z innymi przy pomocy ich kolejek wejsciowych, tzn. tworzy pakiet o konkretnym naglowku, ktory ma za zadanie aktualizacje kanalów.

## Wymogi
Max kientów na serwer: 5

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