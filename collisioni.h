#ifndef PROGETTO_SO___THREAD_COLLISIONI_H
#define PROGETTO_SO___THREAD_COLLISIONI_H

#include "macro.h"

/// Dichiarazione funzioni per le collisioni
void collisione_missili_nemici(Oggetto *enemies, Oggetto *missili, _Bool *primafila);
void collisione_missili_bombe(Oggetto *missili, Oggetto *bombe_nem);
void collisione_bombe_navicella(Oggetto *navicella, Oggetto *bombe_nem);

extern void stampaNemico(Oggetto *enemy);

#endif //PROGETTO_SO___THREAD_COLLISIONI_H
