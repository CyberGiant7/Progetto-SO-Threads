#ifndef PROGETTO_SO___THREAD_MACRO_H
#define PROGETTO_SO___THREAD_MACRO_H
#define _GNU_SOURCE
#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <locale.h>
#include <fcntl.h>
#include <pthread.h>

#define KEY_SPACE 32
#define DIM_NAVICELLA 6
#define DIM_NEMICO 3
#define MAX_MISSILI 10 // Numero (pari) massimo di missili sparabili contemporaneamente

typedef struct {
    int x; /* coordinata x */
    int y; /* coordinata y */
} Pos;
typedef enum{ID_NAVICELLA, ID_NEMICO, ID_MISSILE1, ID_MISSILE2, ID_BOMBA} IdOggetto;
typedef enum {GIOCA, MENU, OPZIONI, ESCI} StatoCorrente;

typedef struct {
    int index;         // Indice dell'oggetto
    IdOggetto id;            // Identificatore dell'entità che invia i dati
    Pos pos;           // Posizione attuale dell'oggetto
    Pos old_pos;       // Posizione precedente dell'oggetto
    pthread_t thread_id;              // Pid del processo proprietario
    int vite;
} Oggetto;

typedef struct {
    Oggetto *navicella;
    _Bool sparo;
} arg_to_navicella;

typedef struct {
    Oggetto *bomba;
    Oggetto *nemico;
} arg_to_bomba;

extern pthread_mutex_t mtx_nave;
extern pthread_mutex_t mtx_missili;
extern pthread_mutex_t mtx_nemici;
extern pthread_mutex_t mtx_bombe;

extern int num_righe;
extern int mov_verticale;

extern int maxx;
extern int maxy;
extern const Oggetto init;
extern int velocita_missili;
extern int M;




#endif //PROGETTO_SO___THREAD_MACRO_H
