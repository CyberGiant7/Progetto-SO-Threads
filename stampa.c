#include "stampa.h"

 char *nave[DIM_NAVICELLA]= {" ▟█▛▀▀",
                             "▟██▙  ",
                             "▗▟█▒▙▖",
                             "▝▜█▒▛▘",
                             "▜██▛  ",
                             " ▜█▙▄▄"};

 char *nemico_lv1[DIM_NEMICO]={"▀█▙",
                               "▒█ ",
                               "▄█▛"};

 char *nemico_lv2[DIM_NEMICO]={" △ ",
                               "◁ ◊",
                               " ▽ "};
 char *nemico_lv3[DIM_NEMICO]= {" ☠ ",
                                "☠ ☠",
                                " ☠ "};

void stampaNavicella(Oggetto *navicella) {
    int i;

    pthread_mutex_lock(&mtx_nave);
    Oggetto temp_navicella = *navicella;
    navicella->old_pos = navicella->pos;
    pthread_mutex_unlock(&mtx_nave);
    /* cancello la 'vecchia' posizione della navicella */
    if (temp_navicella.old_pos.x >= 0) {
        attron(COLOR_PAIR(0));
        for (i = 0; i < DIM_NAVICELLA; i++) {
            mvaddstr(temp_navicella.old_pos.y + i, temp_navicella.old_pos.x, "      ");
        }
        attroff(COLOR_PAIR(0));
    }
    attron(COLOR_PAIR(2));
    for (i = 0; i < DIM_NAVICELLA; i++) {
        if (i <= 1 || i >= 4) {
            attron(COLOR_PAIR(6));
            mvaddstr(temp_navicella.pos.y + i, temp_navicella.pos.x, nave[i]);
            attroff(COLOR_PAIR(6));
        } else {
            attron(COLOR_PAIR(2));
            mvaddstr(temp_navicella.pos.y + i, temp_navicella.pos.x, nave[i]);
            attroff(COLOR_PAIR(2));
        }
    }
}

void stampaNemico(Oggetto *enemy){
    int i;
    Oggetto enemy_temp;
    enemy_temp = *enemy;

    /*Controlla che il nemico sia stato appena inizializzato*/
    if (enemy_temp.old_pos.y != -1 && enemy_temp.old_pos.x != -1) {
        attron(COLOR_PAIR(0));
        for (i = 0; i < DIM_NEMICO; i++) {
            mvprintw(enemy_temp.old_pos.y + i, enemy_temp.old_pos.x, "   "); //cancella la posizione precedente
        }
        attroff(COLOR_PAIR(0));
    }
    /*Controlla quante vite ha il nemico con relativa stampa*/
    switch (enemy_temp.vite) {
        //Stampa nemico livello 1
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
            //Stampa nemico livello 2
        case 2:
            attron(COLOR_PAIR(5));
            for (i = 0; i < DIM_NEMICO; i++) {
                mvprintw(enemy_temp.pos.y + i, enemy_temp.pos.x, nemico_lv2[i]);
            }
            attroff(COLOR_PAIR(1));
            break;
            //Stampa nemico livello 3
        case 1:
            attron(COLOR_PAIR(5));
            for (i = 0; i < DIM_NEMICO; i++) {
                mvprintw(enemy_temp.pos.y + i, enemy_temp.pos.x, nemico_lv3[i]);
            }
            attroff(COLOR_PAIR(1));
            break;
            //Cancella la navicella nemica perchè ha zero vite
        case 0:
            attron(COLOR_PAIR(0));
            for (i = 0; i < DIM_NEMICO; i++) {
                mvprintw(enemy_temp.pos.y + i, enemy_temp.pos.x, "   ");
            }
            attroff(COLOR_PAIR(0));
            break;
    }
}

void stampaBomba(Oggetto *bomba) {
    if (bomba->pos.x != -1 && bomba->pos.y != -1) {
        attron(COLOR_PAIR(0));
        mvprintw(bomba->old_pos.y, bomba->old_pos.x, " ");
        attroff(COLOR_PAIR(0));
    }
    attron(COLOR_PAIR(7));
    mvprintw(bomba->pos.y, bomba->pos.x, "<"); ///♿ ⟢ ⁂ ꗇ ꗈ 💣 🚀 卐 ◌́ ◌͂ ✝
    attroff(COLOR_PAIR(7));
}

void stampaMissile(Oggetto *missile) {
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