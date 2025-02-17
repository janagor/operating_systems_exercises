

Wywołania systemowe:

 * HOLE_MAP
 * WORST_FIT

zostały dodane zgodnie z poleceniem.

* do_worst_fit

    * ustawia wartość alloc_arg - zmiennej systemowej na odpowiedni algorytm:
    0 - domyślny, 1 - worst_fit. W w przypadku innego argumentu robi nic.

    * Zawsze zwraca zero

* do_hole_map
    * argumenty: 
        * bufor na wynik
        * rozmiar bufora
    * działanie:
        * ustawia rozmiar bufora tymczasowego zgodnie z rozmiarem oczekiwanym:
            * tymczasowy bufor tak na prawde zawsze ma rozmiar będący liczbą
            nieparzystą (ostatni znak ma być 0)
        * iteruje po węzłach hp począwszy od hole_head
            * przepisuje odpowiednie wartości : pary rozmiar, adres do bufora
        * wywołuje polecenie sys_copy aby skopiować wartość bufora tymczasowego
        do oczekiwanego
        * zwraca liczbę par uzyskanych

# Modyfikacja alloc_mem

* sprawdza wartość zmiennej systemowej alloc_mem i wybiera algorytm
* jeśli !worst_fit
    standardowe działanie algorytmu
* jeśli worst fit
    * iteruje przez listę w poszukiwaniu miejsca o największym rozmiarze,
    zapamiętuje największy blok i jego poprzednika
    * wybiera blok o największym rozmiarze

