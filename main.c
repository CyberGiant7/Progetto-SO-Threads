/**
 * In questo file viene gestito il cambio di contesto tra menu, opzioni e gioco. Viene gestito il gioco con la funzione
 * @gioco e @AreaGioco.
 *
 *
 *
 *
 */

#include "area_menu.h"
#include "stampa.h"
#include "generazione_movimenti.h"
#include "collisioni.h"


_Bool AreaGioco(Oggetto *navicella, Oggetto *enemies, _Bool *sparo);

/// Inizializzazione di variabili globali
int M = 24;                     // Numero default di nemici
int num_righe = 6;              // Numero default di nemici in ogni colonna
int mov_verticale = 6;          // Numero di spostamenti verticali default
int velocita_missili = 100000;  // Velocità missile default
const Oggetto init = {0, 0, {0, 0}, {0, 0}, 0, 0}; // Costante per inizializzare le strutture Oggetto comodamente

/// Inizializzazione dei mutex utilizzati per i vari thread
pthread_mutex_t mtx_nave = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_missili = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_nemici = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_bombe = PTHREAD_MUTEX_INITIALIZER;

/// Inizializzazione stampe
char *vittoria[6]={"██╗  ██╗ █████╗ ██╗ ██╗   ██╗██╗███╗  ██╗████████╗ █████╗ ",
                   "██║  ██║██╔══██╗██║ ██║   ██║██║████╗ ██║╚══██╔══╝██╔══██╗",
                   "███████║███████║██║ ╚██╗ ██╔╝██║██╔██╗██║   ██║   ██║  ██║",
                   "██╔══██║██╔══██║██║  ╚████╔╝ ██║██║╚████║   ██║   ██║  ██║",
                   "██║  ██║██║  ██║██║   ╚██╔╝  ██║██║ ╚███║   ██║   ╚█████╔╝",
                   "╚═╝  ╚═╝╚═╝  ╚═╝╚═╝    ╚═╝   ╚═╝╚═╝  ╚══╝   ╚═╝    ╚════╝ "};

char *gameover[6]= {" ██████╗  █████╗ ███╗   ███╗███████╗ █████╗ ██╗   ██╗███████╗██████╗ ",
                    "██╔════╝ ██╔══██╗████╗ ████║██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗",
                    "██║  ██╗ ███████║██╔████╔██║█████╗  ██║  ██║╚██╗ ██╔╝█████╗  ██████╔╝",
                    "██║  ╚██╗██╔══██║██║╚██╔╝██║██╔══╝  ██║  ██║ ╚████╔╝ ██╔══╝  ██╔══██╗",
                    "╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗╚█████╔╝  ╚██╔╝  ███████╗██║  ██║",
                    " ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝ ╚════╝    ╚═╝   ╚══════╝╚═╝  ╚═╝"};


int main() {
    StatoCorrente status;
    status = MENU;          // Variabile che conterrà il contesto di esecuzione corrente
    srand(time(NULL));      // Inizializziamo il generatore di numeri random

    /// Gestione cambiamento di stati tra i vari contesti
    while (TRUE) {
        switch (status) {
            case GIOCA:
                status = gioco();
                break;
            case MENU:
                status = menu();
                break;
            case OPZIONI:
                status = opzioni();
                break;
            case ESCI:
                endwin();
                return 0;
        }
        clear();
        refresh();
    }
}

/**
 * Funzione che fa eseguire il gioco
 * @return contesto in cui dovrà tornare una volta terminata la funzione
 */
StatoCorrente gioco() {
    /// Dichiarazione variabili
    int i;                      // Variabile contatore
    _Bool WIN;                  // Contiene se si vince o si perde
    char nome[10];              // Per dare i nomi ai thread (per poter debuggare i thread aperti con pstree)
    pthread_t t_navicella;                                              // Thread id di navicella
    pthread_t *t_enemies = (pthread_t *) calloc(M, sizeof(pthread_t));  // Thread id dei nemici
    Oggetto *enemies = (Oggetto *) calloc(M, sizeof(Oggetto));          // Dati dei nemici

    Oggetto navicella = {0, ID_NAVICELLA, {1, maxy / 2}, {-1, -1}, t_navicella, 3}; // per contenere i dati della navicella
    arg_to_navicella argToNavicella = {&navicella, FALSE};


    setlocale(LC_ALL, "");      // per poter utilizzare caratteri unicode
    initscr();                  // inizializzazione dello schermo
    noecho();                   // i caratteri corrispondenti ai tasti premuti non saranno visualizzati sullo schermo del terminale
    curs_set(0);                // nasconde il cursore

    /// Inizializzazione dei colori
    start_color();
    init_pair(BLACK, COLOR_BLACK, COLOR_BLACK); // per cancellare
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK); // per scrivere
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(RED, COLOR_RED, COLOR_BLACK);


    bkgd(COLOR_PAIR(WHITE));

    ///                                              Inizializzazione treads                                         ///
    /// Thread navicella
    if (pthread_create(&t_navicella, NULL, &nave_player, &argToNavicella)) {
        perror("Errore nella creazione del thread");
        _exit(1);
    }
    navicella.thread_id = t_navicella;
    pthread_setname_np(t_navicella, "Navicella");    // Per dare un nome al thread

    /// Thread nemici
    for (i = 0; i < M; ++i) {
        enemies[i].index = i;                       // Assegnamendo dell'indice
        if (pthread_create(&t_enemies[i], NULL, &Enemy, &enemies[i])) {
            perror("Errore nella creazione del thread");
            _exit(1);
        }
        sprintf(nome, "Nemico_%d", i);
        pthread_setname_np(t_enemies[i], nome);
        enemies[i].thread_id = t_enemies[i];        // Per dare un nome al thread
    }
    /// Invocazione di AreaGioco
    WIN = AreaGioco(&navicella, enemies, &argToNavicella.sparo);

    if (WIN) {
        clear();
        for (i = 0; i < 6; ++i) {
            mvprintw((maxy - 5) / 2 + i, (maxx - 58) / 2, vittoria[i]);     // Stampa vittoria
        }
        refresh();
    } else {
        clear();
        for (i = 0; i < 6; ++i) {
            mvprintw((maxy - 6) / 2 + i, (maxx - 69) / 2, gameover[i]);     // Stampa game over
        }
        refresh();
    }
    sleep(5);
    endwin();
    /// Liberazione delle risorse allocate dinamicamente
    free(t_enemies);
    free(enemies);
    return MENU;    // Dopo la fine del gioco torniamo al menu
}



/// Funzione principale che gestisce tutto il gioco. Da qua vengono gestite le stampe di tutti gli oggetti, le collisioni,
/// la generazione dei thread, e la loro corretta terminazione.
/// \param navicella        Dati di navicella
/// \param enemies          Dati dei nemici
/// \param sparo            Indirizzo della variabile per verificare la pressione del tasto space
/// \return                 Valore di vittoria
_Bool AreaGioco(Oggetto *navicella, Oggetto *enemies, _Bool *sparo) {
    /// Dichiarazione delle variabili
    int i, j = 0;                       // Variabili contatore
    int nemici_vivi;                    // Contatore di nemici vivi
    _Bool sparo_temp;                   // Contenitore temporaneo di sparo
    Oggetto rockets[MAX_MISSILI];       // Array dei dati dei missili amici sparati
    Oggetto bomba_temp;                 // Contenitore temporaneo del valore di bomba
    pthread_t t_missili[MAX_MISSILI];   // Array di thread id dei missili

    /// Inizializzazione variabili
    int punteggio = M * 100;            // Punteggio del gioco
    int missili_vivi = 0;               // Contatore di missili vivi
    int missili_morti = MAX_MISSILI;    // Contatore di missili morti
    _Bool collision = false;            // Per verificare le collisioni con navicella
    _Bool WIN = false;

    /// Inizializzazione variabili dinamiche
    pthread_t *t_bombe_nem = (pthread_t *) calloc(M, sizeof(pthread_t));        // Array di thread id delle bombe
    Oggetto *bombe_nem = (Oggetto *) calloc(M, sizeof(Oggetto));                // Array dei dati delle bombe
    _Bool *primafila = (_Bool *) calloc(M, sizeof(_Bool));                      // Array dei dati di primafila
    arg_to_bomba *argToBomba = (arg_to_bomba *) calloc(M, sizeof(arg_to_bomba));// Array dei dati da passare alle bombe

    /// Inizializzazione primafila
    // Primafila è utilizzato per garantire che siano solo i nemici della prima colonna a sinistra a sparare e non
    // quelli che stanno dietro. Successivamente, appena morirà un nemico della prima fila, a quello dietro verrà
    // impostato il suo valore primafila a true.
    for (i = 0; i < M; ++i) {
        if (i >= (((M / num_righe) - 1) * num_righe) && i < M) {    // Se è in prima fila
            primafila[i] = TRUE;
        } else {
            primafila[i] = FALSE;
        }
    }
    /// Inizializzazione a zero dell'array di missili
    for (i = 0; i < MAX_MISSILI; ++i) {
        rockets[i] = init;
    }
    clear();
    refresh();

    do {    // Inizio del ciclo
        /// Stampa navicella
        stampaNavicella(navicella);
        pthread_mutex_lock(&mtx_nave);
        sparo_temp = *sparo;                // salvo sparo in una variabile temp
        *sparo = false;                     // e lo rendo false
        pthread_mutex_unlock(&mtx_nave);

        if(j == 500) { // Ogni 500 oggetti stampati nello schermo faccio una clear
            clear();
            j = 0;
        }

        /// Generazione e stampa missili
        pthread_mutex_lock(&mtx_missili);
        if (sparo_temp && missili_vivi < MAX_MISSILI) {     // se è stato premuto space e i missili sono sotto il max
            for (int temp = missili_vivi + 2; missili_vivi < temp; missili_vivi++) {
                /// Inizializzo il missile
                if (missili_vivi % 2 == 0)
                    rockets[missili_vivi].id = ID_MISSILE1;
                else
                    rockets[missili_vivi].id = ID_MISSILE2;
                rockets[missili_vivi].pos.x = navicella->pos.x + DIM_NAVICELLA;
                rockets[missili_vivi].pos.y = navicella->pos.y + (DIM_NAVICELLA / 2);
                rockets[missili_vivi].old_pos.x = -1;
                rockets[missili_vivi].old_pos.y = -1;
                rockets[missili_vivi].vite = 1;
                pthread_create(&t_missili[missili_vivi], NULL, &missili, &rockets[missili_vivi]);
                rockets[missili_vivi].thread_id = t_missili[missili_vivi];
            }
            sparo_temp = false;     // diventa false per evitare che al prossimo ciclo rientri qui
        }

        /// Stampa e gestione missili sparabili contemporaneamente
        for (i = 0, missili_morti = 0; i < missili_vivi; ++i) {
            stampaMissile(&rockets[i]);
            if (rockets[i].vite == 0) {     // se il missile è morto
                missili_morti++;
            }
        }
        if (missili_morti == MAX_MISSILI) { // quando muore tutta l'ondata di missili si potrà tornare a sparare
            missili_vivi = 0;
        }
        pthread_mutex_unlock(&mtx_missili);

        /// Stampa dei nemici
        pthread_mutex_lock(&mtx_nemici);
        nemici_vivi = 0;
        for (i = 0; i < M; ++i) {
            if (enemies[i].vite != 0) {
                stampaNemico(&enemies[i]);
                nemici_vivi++;
            }
        }
        pthread_mutex_unlock(&mtx_nemici);

        /// Generazione bombe
        for (i = 0; i < M; ++i) {
            if (primafila[i] == TRUE) {         // se siamo in primafila
                pthread_mutex_lock(&mtx_bombe);
                bomba_temp = bombe_nem[i];      // salvo bomba in una variabile temporanea
                pthread_mutex_unlock(&mtx_bombe);
                if (bomba_temp.vite == 0 && bomba_temp.thread_id == 0) { // se la bomba sparata precedentemente è morta
                    /// Inizializzo parzialmente la nuova bomba
                    pthread_mutex_lock(&mtx_bombe);
                    bombe_nem[i].vite = 1;
                    bombe_nem[i].index = i;
                    pthread_mutex_unlock(&mtx_bombe);
                    argToBomba[i].bomba = &bombe_nem[i];
                    argToBomba[i].nemico = &enemies[i];
                    pthread_create(&t_bombe_nem[i], NULL, &bombe, &argToBomba[i]);
                    bombe_nem[i].thread_id = t_bombe_nem[i];
                }
            }
        }
        /// Stampa bombe
        for (i = 0; i < M; ++i) {
            pthread_mutex_lock(&mtx_bombe);
            if (bombe_nem[i].thread_id != 0) {          // se il thread bomba è stato creato
                if (bombe_nem[i].vite != 0) {           // se la bomba è viva la stampo
                    stampaBomba(&bombe_nem[i]);
                } else if (bombe_nem[i].vite == 0) {    // se è morta la cancello e inizializzo i suoi dati
                    attron(COLOR_PAIR(BLACK));
                    mvprintw(bombe_nem[i].old_pos.y, bombe_nem[i].old_pos.x, " ");
                    attroff(COLOR_PAIR(BLACK));
                    bombe_nem[i] = init;
                }
            }
            pthread_mutex_unlock(&mtx_bombe);
        }
        usleep(1);

        /// Controllo delle varie collisioni
        collisione_missili_nemici(enemies, rockets, primafila);
        collisione_missili_bombe(rockets, bombe_nem);
        collisione_bombe_navicella(navicella, bombe_nem);


        /// Collisione tra navicella e nemici
        pthread_mutex_lock(&mtx_nemici);
        pthread_mutex_lock(&mtx_nave);
        for (i = 0; i < M; i++) {
            if (enemies[i].pos.x == DIM_NAVICELLA + 2) {
                collision = TRUE;
                WIN = FALSE;
                break;
            }
        }
        pthread_mutex_unlock(&mtx_nemici);
        pthread_mutex_unlock(&mtx_nave);

        ///Controllo vite del player
        if (navicella->vite == 0) {
            WIN = FALSE;
            collision = TRUE;
        }
        /// Controllo vite nemici
        if (nemici_vivi == 0) {
            WIN = TRUE;
            collision = true;
        }
        move(0, 0);
        clrtoeol();
        /// Stampa vite e punteggio
        pthread_mutex_lock(&mtx_nave);
        mvprintw(0, 0, "VITE: %d, SCORE: %d", navicella->vite, (punteggio - (nemici_vivi * 100)));
        pthread_mutex_unlock(&mtx_nave);
        j++;
        usleep(10);
        refresh();

    } while (!collision);

    /// Uccisione dei thread dei nemici e delle bombe ancora in vita
    for (i = 0; i < M; ++i) {
        if (enemies[i].thread_id != 0) {
            pthread_cancel(enemies[i].thread_id);
        }
        if (bombe_nem[i].thread_id != 0) {
            pthread_cancel(bombe_nem[i].thread_id);
        }
    }
    /// Uccisione dei thread dei missili ancora in vita
    for (i = 0; i < MAX_MISSILI; i++) {
        if (rockets[i].thread_id != 0) {
            pthread_cancel(rockets[i].thread_id);
        }
    }
    /// Uccisione thread della navicella
    pthread_cancel(navicella->thread_id);

    /// Deallocazione delle risorse allocate dinamicamente
    free(bombe_nem);
    free(primafila);
    free(t_bombe_nem);
    free(argToBomba);
    return WIN;
}