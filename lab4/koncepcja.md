
Rozwiązanie składa się z nastepujących plików:
 * monitor.h - zawiera klasę monitora. Jest kopią udostępnionego pliku na
 stronie przedmiotu przez prowadzącego.
 * zad4.hpp - zawiera deklarację stałej N, klasę PBuffer będącą rozszerzeniem
 klasy Monitor na potrzeby zadania, deklaracje 2 funkcji: producenta (tworzy
 składniki) i konsumenta (tworzy pierogi)
 * zad4.cpp - zawiera implementacje funkcji z zad4.hpp oraz kod programu
 wykonywalnego
 * Makefile - służy do utworzenie programu


Informacje dotyczące rozwiązania:
 * do każdego "surowca" (3*farsz, ciasto) jest przypisany bufor
 * z treści zadania tak na prawdę wynika, że wystarczyłoby aby to była liczba -
 nie przechowujemy żadnej dodatkowej informacji o obiekcie w boforze. Ja 
 wykorzystalem tablicę aby imitować bufor.

Klasa PBufor zawiera następujące pola:
 * typ produktu - użyteczny do logowania
 * dwa warunki: Full i Empty - odpowiedzialne za dostęp do sekcji krytycznej
 * index - index następnego pustego miejsca w buforze: gdy bufor jest pusty
 index == 0, gdy pełny - index == N
 * buffer - bufor na produkty o stałej długości deklarowany w czasie kompilacji

Funkcje:
 * log* - używane wewnątrze sekcji krytycznej: wypisują informacje w celu
 weryfikacji, czy program działa jak należy
 * produce i consume - w tych funckjach jest dostep do sekcji krytycznej,
 odpowiednio inkrementują i dekrementują bufor
 * can_produce i can_consume - sprawdzają czy możliwa jest modyfikacja bufora,
 to bezpośrednio po nich może dojść do oczekiwania lub uwolnienia procesu ze
 stanu oczekiwania na zasoby. Są sprawdzane prze wejściu i wyjściu z sekcji
 krytycznej

Producent:
 * w pętli tworzy zasób, po czym czeka 1s

Konsument:
 * w pętli pobiera ciasto, pobiera farsz, czeka sekundę

Main:
 * przyjęcie liczby producentów i konsumentów
 * alokacją wątków producentów i konsumentów wraz z ich uruchomieniem


