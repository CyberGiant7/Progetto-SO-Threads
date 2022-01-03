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

StatoCorrente gioco();
_Bool AreaGioco(Oggetto *navicella, Oggetto *enemies, arg_to_navicella *argToNavicella);

int M = 24; //numero default di nemici
int num_righe = 6; //numero default di nemici in ogni colonna
int mov_verticale = 6; // numero di spostamenti verticali default
int velocita_missili = 100000; //velocità missile default


pthread_mutex_t mtx_nave = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_missili = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_nemici = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_bombe = PTHREAD_MUTEX_INITIALIZER;


const Oggetto init = {0, 0, {0, 0}, {0, 0}, 0, 0}; //Costante utilizzata per inizializzare la struttura OGGETTO

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
    status = MENU;
    srand(time(NULL)); /* inizializziamo il generatore di numeri random */

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
 *
 * @return
 */
StatoCorrente gioco() {
    int i, l, n;
    char nome[10];
    pthread_t t_navicella;
    pthread_t *t_enemies = (pthread_t*) calloc(M, sizeof(pthread_t));

    Oggetto navicella = {0, ID_NAVICELLA, {1, maxy/2}, {-1, -1}, t_navicella, 3};
    Oggetto *enemies = (Oggetto *) calloc(M, sizeof(Oggetto));
    arg_to_navicella argToNavicella = {&navicella, FALSE};
    _Bool WIN;
    l = 0, n = 0;

    setlocale(LC_ALL, "");
    initscr(); /* inizializzazione dello schermo */
    noecho(); /* i caratteri corrispondenti ai tasti premuti non saranno visualizzati sullo schermo del terminale */
    curs_set(0); /* nasconde il cursore */

    start_color();
    init_pair(0, COLOR_BLACK, COLOR_BLACK); // per cancellare
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // per scrivere
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_RED, COLOR_BLACK);

    bkgd(COLOR_PAIR(1));

    //inizializzazione treads
    /// thread navicella
    if(pthread_create(&t_navicella, NULL, &nave_player, &argToNavicella)){
        perror("errore nella creazione del thread");
        _exit(1);
    }
    navicella.thread_id = t_navicella;
    pthread_setname_np(t_navicella,"navicella");
    /// thread nemici
    for (i = 0; i < M; ++i) {
        enemies[i].index = i;
        if (pthread_create(&t_enemies[i], NULL, &Enemy, &enemies[i])) {
            perror("errore nella creazione del thread");
            _exit(1);
        }
        sprintf(nome, "Nemico_%d", i);
        pthread_setname_np(t_enemies[i], nome);
        enemies[i].thread_id = t_enemies[i];
    }
    WIN = AreaGioco(&navicella, enemies, &argToNavicella); /* il processo padre invoca la funzione di AreaGioco */

    if(WIN){
        clear();
        for (i = 0; i < 6; ++i) {
            mvprintw((maxy- 5)/2 + i, (maxx-58)/2, vittoria[i]);
        }
        refresh();
    }else {
        clear();
        for (i = 0; i < 6; ++i) {
            mvprintw((maxy- 6)/2 + i, (maxx-69)/2, gameover[i]);
        }
        refresh();
    }
    sleep(5);
    endwin();
    free(t_enemies);
    free(enemies);
    return MENU;
}




_Bool AreaGioco(Oggetto *navicella, Oggetto *enemies, arg_to_navicella *argToNavicella) {
    clear();
    refresh();
    int i, j;
    int punteggio = M * 100;
    int nemici_vivi = M, missili_vivi = 0, missili_morti = MAX_MISSILI;
    _Bool collision = false, WIN = false, sparo;
    Oggetto valore_letto = init;
    Oggetto rockets[MAX_MISSILI];
    Oggetto bomba_temp;
    pthread_t *t_bombe_nem = (pthread_t *) calloc(M, sizeof(pthread_t));
    Oggetto *bombe_nem = (Oggetto *) calloc(M, sizeof(Oggetto));
    _Bool *primafila = (_Bool *) calloc(M, sizeof(_Bool));
    arg_to_bomba *argToBomba = (arg_to_bomba *) calloc(M, sizeof(arg_to_bomba));
    pthread_t t_missili[MAX_MISSILI];

    /// Inizializzazione primafila
    for (i = 0; i < M; ++i) {
        if (i >= (((M / num_righe) - 1) * num_righe) && i < M) {
            primafila[i] = TRUE;
        } else {
            primafila[i] = FALSE;
        }
    }
    for (i = 0; i < MAX_MISSILI; ++i) {
        rockets[i] = init;
    }

    do {
        /// Stampa navicella
        stampaNavicella(navicella);
        pthread_mutex_lock(&mtx_nave);
        sparo = argToNavicella->sparo;
        argToNavicella->sparo = false;
        pthread_mutex_unlock(&mtx_nave);

        /// Generazione e stampa missili
        pthread_mutex_lock(&mtx_missili);
        if (sparo && missili_vivi < MAX_MISSILI) {
            for (int temp = missili_vivi + 2; missili_vivi < temp; missili_vivi++) {
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
            sparo = false;
        }
        /// Gestione numero di missili sparabili in contemporanea
        for (i = 0, missili_morti = 0; i < missili_vivi; ++i) {
            stampaMissile(&rockets[i]);
            if (rockets[i].vite == 0) {
                missili_morti++;
                rockets[i] = init;
            }
        }
        if (missili_morti == MAX_MISSILI) {
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
            if (primafila[i] == TRUE) {
                pthread_mutex_lock(&mtx_bombe);
                bomba_temp = bombe_nem[i];
                pthread_mutex_unlock(&mtx_bombe);
                if (bomba_temp.vite == 0 && bomba_temp.thread_id == 0) {
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
            if (bombe_nem[i].thread_id != 0) {
                if (bombe_nem[i].vite != 0) {
                    stampaBomba(&bombe_nem[i]);
                } else if (bombe_nem[i].vite == 0) {
                    attron(COLOR_PAIR(0));
                    mvprintw(bombe_nem[i].old_pos.y, bombe_nem[i].old_pos.x, " ");
                    attroff(COLOR_PAIR(0));
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

        if (nemici_vivi == 0) {
            WIN = TRUE;
            collision = true;
        }
        move(0, 0);
        clrtoeol();
        usleep(100);
        pthread_mutex_lock(&mtx_nave);
        mvprintw(0, 0, "VITE: %d, SCORE: %d", navicella->vite, (punteggio - (nemici_vivi * 100)));
        pthread_mutex_unlock(&mtx_nave);
        refresh();
    } while (!collision);
    /// uccisione dei thread dei nemici
    for (i = 0; i < M; ++i) {
        if (enemies[i].thread_id != 0) {
            pthread_cancel(enemies[i].thread_id);
        }
        if (bombe_nem[i].thread_id != 0) {
            pthread_cancel(bombe_nem[i].thread_id);
        }
    }
    /// uccisione dei trhread dei missili
    for (i = 0; i < MAX_MISSILI; i++) {
        if (rockets[i].thread_id != 0) {
            pthread_cancel(rockets[i].thread_id);
        }
    }
    /// Uccisione thread della navicella
    pthread_cancel(navicella->thread_id);
    free(bombe_nem);
    free(primafila);
    return WIN;
}