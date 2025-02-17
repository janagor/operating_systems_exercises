
Rozwiązanie składa się z nastepujących plików:
 * zad3.h - zawiera deklarację stałej N, struktury parametrów wątku oraz
 Bufora i deklaracje funkcji używanych w programie
 * zad3.c - zawiera implementacje funkcji z zad3.h oraz kod programu
 wykonywalnego
 * Makefile - służy do utworzenie programu


Informacje dotyczące rozwiązania:
 * do każdego "surowca" (3*farsz, ciasto) jest przypisany bufor
 * żeby skrócić liczbe linijek kodu mamy tablicę 4-elementową buforów
 * z treści zadania tak na prawdę wynika, że wystarczyłoby aby to była liczba -
 nie przechowujemy żadnej dodatkowej informacji o obiekcie w boforze. Ja 
 wykorzystalem tablicę aby imitować bufor.

Struktura Bufor zawiera następujące pola:
 * index - index następnego pustego miejsca w buforze: gdy bufor jest pusty
 index == 0, gdy pełny - index == N
 * values - bufor na produkty o stałej długości deklarowany w czasie kompilacji

Zmienne globalne:
 * bufory
 * semafory empty i full dla kazdego bufora
 * mutexy dla każdego bufora

Funkcje:
 * init buffers - inicjuje bufory
 * log* - wyswietla informacje debugowe w celu sprawdzania dzialania programu


Producent:
 * w pętli wchodzi do sekcji krytycznej tworzy zasób, wyswietla informacje
 debugowe, śpi 1s

Konsument:
 * w pętli wchodzi do sekcji krytycznej ciasta, pobiera ciasto, wychodzi,
 wchodzi do sekcji krytycznej farsze, pobiera farsz, loguje, wychodzi,
 czeka sekundę

Main:
 * przyjęcie liczby producentów i konsumentów
 * alokacją wątków producentów i konsumentów wraz z ich uruchomieniem


