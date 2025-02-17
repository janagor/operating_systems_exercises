# Koncepcja

Rozwiązanie zawiera 4 pliki:

 * zad6.hpp - zawiera stałe i klasy Inode oraz VirtualDisk
 * zad6.cpp - zawiera implementacje metod z klas Inode oraz VirtualDisk
 * app.cpp - przykładowa aplikacja do obsługi dysków
 * Makefile - plik do wygenerowania programu app

Struktura reprezentująca dysk przyjmujący formę jednopoziomowego katalogu
posiada następujące atrybuty:

* strukturę (strumień) pozwalający na komunikację z plikiem dysku,
* tablicę dynamiczną (wektor) obiektów Inode
* tablicę dynamiczną (wektor) informacji o blokach dysku - czy jest zajęty
* nazwę dysku użyteczną do tworzenia i usuwania dysku.

Strukura Inode przechowuje informację dotyczącą pliku na dysku. Jest tworzona
przy wczytywaniu nowego pliku i usuwana przy usunięciu dysku. pozwala na bycie
zserializowanym. Zawiera następujące pola:

* nazwa pliku
* rozmiar pliku
* pierwszy blok zajęty przez plik
* liczbę bloków zajmowanych przez plik

Pliki są zapisywane do pierwszego odpowiadającego rozmiarem miejsca na dysku.
Przyjęty został sposób typ alokacji ciągłej.



