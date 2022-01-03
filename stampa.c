#include "stampa.h"
/// Inizializzazione grafica della navicella del giocatore
 char *nave[DIM_NAVICELLA]= {" ▟█▛▀▀",
                             "▟██▙  ",
                             "▗▟█▒▙▖",
                             "▝▜█▒▛▘",
                             "▜██▛  ",
                             " ▜█▙▄▄"};
/// Inizializzazione grafica della navicella nemica di livello 1
 char *nemico_lv1[DIM_NEMICO]={"▀█▙",
                               "▒█ ",
                               "▄█▛"};
/// Inizializzazione grafica della navicella nemica di livello 2
 char *nemico_lv2[DIM_NEMICO]={" △ ",
                               "◁ ◊",
                               " ▽ "};
/// Inizializzazione grafica della navicella nemica di livello 3
 char *nemico_lv3[DIM_NEMICO]= {" ☠ ",
                                "☠ ☠",
                                " ☠ "};
/**
 *  La funzione @stampaNavicella stampa la navicella del giocatore nella posizione corrente e cancella dallo schermo
 *  la stampa relativa alla posizione precedente in cui si trovava la navicella. Il suo parametro in ingresso è:
 * @param navicella - È una struttura @Oggetto che contiene tutti i dati relativi alla navicella del giocatore.
 */
void stampaNavicella(Oggetto *navicella) {
    int i; // indice ciclo for

    pthread_mutex_lock(&mtx_nave); ///blocca il mutex @mtx_nave
    Oggetto temp_navicella = *navicella; /// variabile temporanea per evitare di invocare ripetutamente il mutex @mtx_nave
    navicella->old_pos = navicella->pos; // salva la vecchia posizione della navicella del giocatore
    pthread_mutex_unlock(&mtx_nave); ///sblocca il mutex @mtx_nave
    /* Cancella la 'vecchia' posizione della navicella */
    if (temp_navicella.old_pos.x >= 0) {
        attron(COLOR_PAIR(0));
        for (i = 0; i < DIM_NAVICELLA; i++) {
            mvaddstr(temp_navicella.old_pos.y + i, temp_navicella.old_pos.x, "      ");
        }
        attroff(COLOR_PAIR(0));
    }
    attron(COLOR_PAIR(2));
    /* Stampa la 'nuova' posizione della navicella*/
    for (i = 0; i < DIM_NAVICELLA; i++) {
        if (i <= 1 || i >= 4) {
            // Colora le 'parti esterne' della navicella
            attron(COLOR_PAIR(6));
            mvaddstr(temp_navicella.pos.y + i, temp_navicella.pos.x, nave[i]);
            attroff(COLOR_PAIR(6));
        } else {
            // Colora la 'parte centrale' della navicella
            attron(COLOR_PAIR(2));
            mvaddstr(temp_navicella.pos.y + i, temp_navicella.pos.x, nave[i]);
            attroff(COLOR_PAIR(2));
        }
    }
}
/**
 *  La funzione @stampaNemico stampa le navicelle nemiche nella posizione corrente e cancella dallo schermo
 *  la stampa relativa alla posizione in cui si trovavano precedentemente. Il suo parametro in ingresso è:
 * @param enemy - È una struttura @Oggetto che contiene tutti i dati relativi alla singola navicella nemica.
 */
void stampaNemico(Oggetto *enemy){
    int i; // indice ciclo for
    Oggetto enemy_temp; // variabile temporanea per salvare i dati della navicella da stampare
    enemy_temp = *enemy; //inzializza la variabile temporanea

    /** Se la navicella nemica è stata appena inizializzata **/
    if (enemy_temp.old_pos.y != -1 && enemy_temp.old_pos.x != -1) {
        attron(COLOR_PAIR(0));
        for (i = 0; i < DIM_NEMICO; i++) {
            mvprintw(enemy_temp.old_pos.y + i, enemy_temp.old_pos.x, "   "); //cancella la posizione precedente
        }
        attroff(COLOR_PAIR(0));
    }
    /** Controlla quante vite ha il nemico con relativa stampa **/
    switch (enemy_temp.vite) {
        // Se ha tre vite stampa nemico livello 1
        case 3:
            for (i = 0; i < DIM_NEMICO; i++) {
                if (i == 1) {
                    attron(COLOR_PAIR(4));
                    mvprintw(enemy_temp.pos.y + i, enemy_temp.pos.x, nemico_lv1[i]);
                    attroff(COLOR_PAIR(4));
                } else {
                    attron(COLOR_PAIR(7));
                    mvprintw(enemy_temp.pos.y + i, enemy_temp.pos.x, nemico_lv1[i]);
                    attroff(COLOR_PAIR(7));
                }
            }
            break;
            // Se ha due vite stampa nemico livello 2
        case 2:
            attron(COLOR_PAIR(5));
            for (i = 0; i < DIM_NEMICO; i++) {
                mvprintw(enemy_temp.pos.y + i, enemy_temp.pos.x, nemico_lv2[i]);
            }
            attroff(COLOR_PAIR(1));
            break;
            // Se ha una vita stampa nemico livello 3
        case 1:
            attron(COLOR_PAIR(5));
            for (i = 0; i < DIM_NEMICO; i++) {
                mvprintw(enemy_temp.pos.y + i, enemy_temp.pos.x, nemico_lv3[i]);
            }
            attroff(COLOR_PAIR(1));
            break;
            // Se ha zero vite cancella la navicella nemica
        case 0:
            attron(COLOR_PAIR(0));
            for (i = 0; i < DIM_NEMICO; i++) {
                mvprintw(enemy_temp.pos.y + i, enemy_temp.pos.x, "   ");
            }
            attroff(COLOR_PAIR(0));
            break;
    }
}
/**
 *  La funzione @stampaBomba stampa le bombe nemiche nella posizione corrente e cancella dallo schermo
 *  la stampa relativa alla posizione in cui si trovavano precedentemente. Il suo parametro in ingresso è:
 * @param bomba - È una struttura @Oggetto che contiene tutti i dati relativi alla singola bomba nemica.
 */
void stampaBomba(Oggetto *bomba) {
    // Se la bomba è all'interno dalla finestra di gioco
    if (bomba->pos.x != -1 && bomba->pos.y != -1) {
        attron(COLOR_PAIR(0));
        mvprintw(bomba->old_pos.y, bomba->old_pos.x, " "); // cancella la stampa dell'ultima posizione della bomba
        attroff(COLOR_PAIR(0));
    }
    /* Stampa la bomba nella posizione corrente*/
    attron(COLOR_PAIR(7));
    mvprintw(bomba->pos.y, bomba->pos.x, "⁂"); ///♿ ⟢ ⁂ ꗇ ꗈ 💣 🚀 卐 ◌́ ◌͂ ✝
    attroff(COLOR_PAIR(7));
}
/**
 * La funzione @stampaMissile stampa i missili nella posizione corrente e cancella dallo schermo
 *  la stampa relativa alla posizione in cui si trovavano precedentemente. Il suo parametro in ingresso è:
 * @param missile - È una struttura @Oggetto che contiene tutti i dati del singolo missile generato dalla
 *                  navicella del giocatore.
 */
void stampaMissile(Oggetto *missile) {
    /* Se il missile è all'interno dalla finestra di gioco*/
    if (missile->pos.x != -1 && missile->pos.y != -1) {
        attron(COLOR_PAIR(0));
        mvprintw(missile->old_pos.y, missile->old_pos.x, " ");
        attroff(COLOR_PAIR(0));
    }
    attron(COLOR_PAIR(3));
    mvprintw(missile->pos.y, missile->pos.x, "⟢"); ///♿ ⟢ ⁂ ꗇ ꗈ 💣 🚀 卐 ◌́ ◌͂ ✝
    attroff(COLOR_PAIR(3));

    missile->old_pos = missile->pos;
}