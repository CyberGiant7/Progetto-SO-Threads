progetto_thread: main.o area_menu.o collisioni.o generazione_movimenti.o stampa.o
	gcc -o progetto_thread main.o area_menu.o collisioni.o generazione_movimenti.o stampa.o -lncursesw -lpthread

main.o: area_menu.h stampa.h generazione_movimenti.h collisioni.h main.c
	gcc -std=c99 -c main.c

area_menu.o: area_menu.h area_menu.c
	gcc -std=c99 -c  area_menu.c

collisioni.o: macro.h collisioni.h collisioni.c
	gcc -std=c99 -c  collisioni.c

generazione_movimenti.o: macro.h generazione_movimenti.h generazione_movimenti.c
	gcc -std=c99 -c  generazione_movimenti.c

stampa.o: macro.h stampa.h stampa.c
	gcc -std=c99 -c  stampa.c

clean:
	rm *.o

depencies:
	if ! dpkg -l | grep libncursesw5-dev -c >>/dev/null; then echo "osboxes.org" | sudo -S apt-get -y install libncursesw5-dev; fi