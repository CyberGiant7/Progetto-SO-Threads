#include "collisioni.h"

void collisione_missili_bombe(Oggetto *missili, Oggetto *bombe_nem){
    int i, j;
    pthread_mutex_lock(&mtx_missili);
    pthread_mutex_lock(&mtx_bombe);
    /// collissione tra missili e bombe
    for (i = 0; i < MAX_MISSILI; i++) {
        if (missili[i].thread_id != 0 && missili[i].vite != 0) {
            for (j = 0; j < M; j++) {
                if (bombe_nem[j].thread_id != 0 && bombe_nem[j].vite != 0) {
                    if (missili[i].pos.x == bombe_nem[j].pos.x && missili[i].pos.y == bombe_nem[j].pos.y){
                        //uccido il missile colliso

                        pthread_cancel(missili[i].thread_id);
                        missili[i].vite = 0;

                        //uccido la bomba collisa
                        pthread_cancel(bombe_nem[j].thread_id);
                        bombe_nem[j].vite = 0;
                        attron(COLOR_PAIR(0));
                        mvaddstr(missili[i].old_pos.y, missili[i].old_pos.x, " ");
                        mvaddstr(missili[i].pos.y, missili[i].pos.x, " ");
                        attroff(COLOR_PAIR(0));
                        missili[i] = init;
                    }
                }
            }
        }
    }
    pthread_mutex_unlock(&mtx_missili);
    pthread_mutex_unlock(&mtx_bombe);
}
void collisione_bombe_navicella(Oggetto *navicella, Oggetto *bombe_nem){
    int i,j,k;
    pthread_mutex_lock(&mtx_nave);
    pthread_mutex_lock(&mtx_bombe);

    ///collisione tra bombe e navicella player
    for (i = 0; i < M; i++) {
        if (bombe_nem[i].thread_id != 0) {
            for (j = 0; j < DIM_NAVICELLA; ++j) {
                for (k = 0; k < DIM_NAVICELLA; ++k) {
                    if (j > 0 && j<5){
                        k = 5;
                    }
                    if(bombe_nem[i].pos.x == navicella->pos.x + k && bombe_nem[i].pos.y == navicella->pos.y + j){
//                          uccido la bomba collisa
                        pthread_cancel(bombe_nem[i].thread_id);
                        printf("\a");
                        fflush(stdout);
                        bombe_nem[i].vite = 0;
                        navicella->vite--;
                    }
                }
            }
        }
    }
    pthread_mutex_unlock(&mtx_nave);
    pthread_mutex_unlock(&mtx_bombe);
}
void collisione_missili_nemici(Oggetto *enemies, Oggetto *missili, _Bool *primafila) {
    pthread_mutex_lock(&mtx_nemici);
    pthread_mutex_lock(&mtx_missili);
    int i, j, k, l;
    ///collisione tra Missili e Nemici
    for (i = 0; i < M; i++) {
        if (enemies[i].vite != 0) {
            for (j = 0; j < MAX_MISSILI; j++) {
                if (missili[j].vite != 0) {
                    for (k = 0; k < DIM_NEMICO; ++k) {
                        for (l = 0; l < DIM_NEMICO; ++l) {
                            if (missili[j].pos.x == enemies[i].pos.x + l && missili[j].pos.y == enemies[i].pos.y + k) {
                                pthread_cancel(missili[j].thread_id);
                                missili[j].vite = 0;
                                enemies[i].vite -= 1;

                                if ((i - num_righe) >= 0 && enemies[i].vite == 0){
                                    primafila[i-num_righe] = TRUE;
                                }
                                attron(COLOR_PAIR(0));
                                mvaddstr(missili[j].old_pos.y, missili[j].old_pos.x, " ");
                                attroff(COLOR_PAIR(0));
                                missili[j] = init;
                                if (enemies[i].vite == 0){
                                    pthread_cancel(enemies[i].thread_id);
                                    stampaNemico(&enemies[i]); //cancello il nemico morto
                                    enemies[i] = init;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    pthread_mutex_unlock(&mtx_nemici);
    pthread_mutex_unlock(&mtx_missili);
}
