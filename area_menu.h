#ifndef PROGETTO_SO___THREAD_AREA_MENU_H
#define PROGETTO_SO___THREAD_AREA_MENU_H
#include "macro.h"



#define MENU_ELEM 3
#define DEFAULT_MAXX 100
#define DEFAULT_MAXY 30
#define DIM_TITOLO 13


#define NUMERO_OPZIONI 4
#define NUMERO_RISOLUZIONI 5
#define NUMERO_SELEZIONI_NEMICI 12
#define NUMERO_VELOCITA_MISSILI 3
#define MENUWIN_X 70
#define MENUWIN_Y 20



StatoCorrente menu();
StatoCorrente gioco();


void stampaNemico(Oggetto *enemy);
void stampaNavicella(Oggetto *navicella);
void stampaMissile(Oggetto *missile);
void stampaBomba(Oggetto *bomba);


StatoCorrente opzioni();

#endif //PROGETTO_SO___THREAD_AREA_MENU_H
