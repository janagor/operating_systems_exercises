
Dodano wywołanie systemowe SETPRI, które przyjmuje 3 argumenty:
 * pid procesu który modfikujemy
 * wartość grupy procesów do której chcemy przypisać proces
 * wartość dodatkowa - dla grupy round-robin nieistotna, dla grupy "starzenie"
 wiek bazowy, dla grupy z czasem działania, deklarowana wartość czasu działania

Dodano do struktury procesu 3 pola: wiek, czas (do ukończenia), id grupy

W procedurze main w pliku /usr/src/kernel/main.c dodano inicjacje zmiennych
dla procesu init

W pliku /usr/src/kernel/proc.c znajdują się główne modyfikacje dotyczące
zadania.

Modyfikowane były dwie procedury: ready i sched.

sched:
 * wybieramy proces który był teraz wykonywany i wstawiamy na odpowiednie
 miejsce w liscie procesów
    * jeśli proces jest nullem kończymy
    * jeśli nie null: sprawdzamy grupę tego procesu i przechodzimy do obslugi
    * danej grupy
        * round-robin to niemalże to samo co w domyślnym, z tą różnicą, że
        proces skończy na końcu kolejki procesów swojej grupy a nie na końcu
        całej kolejki
        * starzenie: każdy proces który był w kolejce się postarzał, więc każdy
        się starzeje
        * wstawiamy proces do odpowiedniego miejsca
        * czas zakończenia: dekrementujemy czas obecnie zakonczonego procesu,
        wstawiamy do odpowiedniego miejsca w kolejce
ready:
 * tak na prawdę jeśli chodzi o samą implementację, to różni się ona od zmian w
 procedurze sched tym, że nie modyfikuję pól procesu, i zamiast pobierać proces
 z głowy procesów jest on podany jako argument procedury

Testowanie polega na sprawdzeniu następujących scenariuszy:
 * startujemy z najwyższego priorytetu, uruchamiamy proces rodzica, tworzymy
 dziecko tego procesu poprzed fork(), ojciec i dziecko zmieniają swoją grupę,
    * ojciec w pętli nic nie robi, wyswietla komunikat że zakończył działanie, 
 czeka 1s i kończy działanie
    * dziecko wyswietla komunikat o zakonczeniu dzialania i konczy dzialanie

 1. ojciec ma zawsze wiekszą grupę priorytetu niż dziecko
 2. ojciec ma zawsze mniejszą grupę priorytetu niż dziecko
 3. ta sama grupa priorytetu, ale ojciec ma "lepsze" wartości startowe (starszy
 wiek lub mniejszy czas potrzebny do zakonczenia)
 4. ta sama grupa priorytetu, ale ojciec ma "gorsze" wartości startowe (młodszy
 wiek lub większy czas potrzebny do zakonczenia)


