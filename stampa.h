#ifndef PROGETTO_SO___THREAD_STAMPA_H
#define PROGETTO_SO___THREAD_STAMPA_H

#include "macro.h"

///Dichiarazione variabili globali per la stampa
extern char *nave[DIM_NAVICELLA];
extern char *nemico_lv1[DIM_NEMICO];
extern char *nemico_lv2[DIM_NEMICO];
extern char *nemico_lv3[DIM_NEMICO];

/// Dichiarazione delle funzioni di stampa
void stampaNavicella(Oggetto *navicella);
void stampaNemico(Oggetto *enemy);
void stampaBomba(Oggetto *bomba);
void stampaMissile(Oggetto *missile);

#endif //PROGETTO_SO___THREAD_STAMPA_H
