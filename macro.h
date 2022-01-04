/**
 * In questo file vengono dichiarate tutte variabili globali, strutture, enumerazioni e macro utilizzate in tutti i
 * files.
 */

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

/// Colori
#define BLACK 0
#define WHITE 1
#define GREEN 2
#define YELLOW 3
#define MAGENTA 4
#define CYAN 5
#define BLUE 6
#define RED 7

#define KEY_SPACE 32        // Id tasto space
#define DIM_NAVICELLA 6     // Dimensione navicella (caratteri)
#define DIM_NEMICO 3        // Dimensione nemici    (caratteri)
#define MAX_MISSILI 10      // Numero (pari) massimo di missili sparabili contemporaneamente

/// Struttura per salvare le coordinate
typedef struct {
    int x; /* coordinata x */
    int y; /* coordinata y */
} Pos;

/// Enumerazione per gestire gli ID dei diversi oggetti
typedef enum{ID_NAVICELLA, ID_NEMICO, ID_MISSILE1, ID_MISSILE2, ID_BOMBA} IdOggetto;

/// Struttura per descrivere i vari oggetti del gioco
typedef struct {
    int index;              // Indice dell'oggetto
    IdOggetto id;           // Identificatore dell'entità che invia i dati
    Pos pos;                // Posizione attuale dell'oggetto
    Pos old_pos;            // Posizione precedente dell'oggetto (utile per cancellare)
    pthread_t thread_id;    // Thread id del thread proprietario
    int vite;               // Numero di vite dell'oggetto
} Oggetto;

/// Enumerazione per gestire i vari cambi di contesto
typedef enum {GIOCA, OPZIONI, ESCI, MENU} StatoCorrente;

/// Struttura per comunicare con il thread della navicella
typedef struct {
    Oggetto *navicella;     // Dati della navicella (per muoversi)
    _Bool sparo;            // Variabile per verificare che spazio sia stato premuto
} arg_to_navicella;

/// Struttura per comunicare con il thread delle bombe
typedef struct {
    Oggetto *bomba;         // Dati della bomba (per muoversi)
    Oggetto *nemico;        // Dati del nemico che ha generato la bomba (per inizializzare la posizione della bomba)
} arg_to_bomba;

/// Dichiarazione dei vari mutex
extern pthread_mutex_t mtx_nave;
extern pthread_mutex_t mtx_missili;
extern pthread_mutex_t mtx_nemici;
extern pthread_mutex_t mtx_bombe;

/// Dichiarazione di variabili globali
extern int num_righe;       // Numero di nemici che verranno stampati in ogni colonna
extern int mov_verticale;   // Numero di spostamenti che i nemici effettuano prima di sbattere nel bordo

extern int maxx;            // Risoluzione dello schermo (numero di colonne)
extern int maxy;            // Risoluzione dello schermo (numero di righe)
extern const Oggetto init;  // Costante utilizzata per inizializzare le strutture Oggetto comodamente
extern int velocita_missili;// Velocità dei missili (valore inserito in un usleep)
extern int M;               // Numero di nemici generati




#endif //PROGETTO_SO___THREAD_MACRO_H
