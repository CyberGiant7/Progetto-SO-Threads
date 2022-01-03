#include "generazione_movimenti.h"

void* Enemy(void* arg) {
    int j = 0, l = 0, temp;
    int status = 0;

    Oggetto *enemy, temp_enemy;
    enemy = (Oggetto*) arg;

    pthread_mutex_lock(&mtx_nemici);
    enemy->vite = 3;
    enemy->old_pos.x = -1;
    enemy->old_pos.y = -1;
    enemy->id = ID_NEMICO;
    temp_enemy = *enemy;

    pthread_mutex_unlock(&mtx_nemici);
    while(true) {
        temp_enemy.pos.y = (1 + (temp_enemy.index % num_righe) * (DIM_NEMICO + 1)) + j;
        temp = (int) (temp_enemy.index / num_righe) + 1;
        temp_enemy.pos.x = maxx - temp * (DIM_NEMICO + 1) - l;


        if (temp_enemy.vite == 0){
            pthread_mutex_lock(&mtx_nemici);
            enemy->pos = temp_enemy.pos;
            enemy->old_pos = temp_enemy.old_pos;
            pthread_mutex_unlock(&mtx_nemici);
            pthread_exit(NULL);
        } else {
            pthread_mutex_lock(&mtx_nemici);
            enemy->pos = temp_enemy.pos;
            enemy->old_pos = temp_enemy.old_pos;
            temp_enemy.vite = enemy->vite;
            pthread_mutex_unlock(&mtx_nemici);
        }


        /*Spostamenti navicelle*/
        if (j % 8 == 0) {
            l++;
        }

        usleep(300000); //velocità spostamento nemici
        if (j <= mov_verticale && status == 0) {
            j++;
            if (j == mov_verticale)
                status = 1;
        } else if (j >= 0 && status == 1) {
            j--;
            if (j == 0)
                status = 0;
        }
        temp_enemy.old_pos = temp_enemy.pos;
    }
}

void* nave_player(void *arg) {
    arg_to_navicella *argToNavicella;
    argToNavicella = (arg_to_navicella *) arg;
    Oggetto *navicella = argToNavicella->navicella;
    _Bool *sparo = &argToNavicella->sparo;

    keypad(stdscr, TRUE);
    while (TRUE) {
        int c = getch();
        switch (c) {
            case KEY_UP:
                pthread_mutex_lock(&mtx_nave);
                if (navicella->pos.y > 1)
                    navicella->pos.y -= 1;
                pthread_mutex_unlock(&mtx_nave);
                break;
            case KEY_DOWN:
                pthread_mutex_lock(&mtx_nave);
                if (navicella->pos.y < maxy - DIM_NAVICELLA)
                    navicella->pos.y += 1;
                pthread_mutex_unlock(&mtx_nave);
                break;
            case KEY_SPACE:
                pthread_mutex_lock(&mtx_nave);
                *sparo = true;
                pthread_mutex_unlock(&mtx_nave);
                break;
        }
    }
}

void* missili(void* arg) {
    Oggetto *missile, temp_missile;
    missile = (Oggetto *) arg;
    int i = 0;
    while (TRUE) {
        pthread_mutex_lock(&mtx_missili);
        temp_missile = *missile;
        pthread_mutex_unlock(&mtx_missili);
        if (i % 20 == 0) {
            if (temp_missile.id == ID_MISSILE2) {
                temp_missile.pos.y += 1;
            } else if (temp_missile.id == ID_MISSILE1) {
                temp_missile.pos.y -= 1;
            }
        }
        if (temp_missile.id == ID_MISSILE2 || temp_missile.id == ID_MISSILE1) {
            temp_missile.pos.x += 1;
        }
        i++;
        if ((temp_missile.pos.x > maxx || temp_missile.pos.y >= maxy || temp_missile.pos.y < 0) &&
            (temp_missile.id == ID_MISSILE2 || temp_missile.id == ID_MISSILE1)) {
            temp_missile.vite = 0;
            pthread_mutex_lock(&mtx_missili);
            *missile = init;
            pthread_mutex_unlock(&mtx_missili);
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&mtx_missili);
        *missile = temp_missile;
        pthread_mutex_unlock(&mtx_missili);
        usleep(velocita_missili); //regola velocità missili
    }
}

void* bombe(void* arg) {
    Oggetto *bomba, temp_bomba;
    arg_to_bomba *argToBomba;
    argToBomba = (arg_to_bomba*) arg;
    bomba = argToBomba->bomba;

    if (bomba->index % 4 == 0) {
        sleep(1);
    } else if (bomba->index % 2 == 1) {
        sleep(3);
    } else if (bomba->index % 2 == 0) {
        sleep(5);
    }
    pthread_mutex_lock(&mtx_bombe);
    bomba->id = ID_BOMBA;
    pthread_mutex_lock(&mtx_nemici);
    bomba->pos.x = argToBomba->nemico->pos.x-1;
    bomba->pos.y = argToBomba->nemico->pos.y + (DIM_NEMICO/2);
    pthread_mutex_unlock(&mtx_nemici);
    bomba->old_pos.x = -1;
    bomba->old_pos.y = -1;
    bomba->vite = 1;
    pthread_mutex_unlock(&mtx_bombe);

    pthread_mutex_lock(&mtx_bombe);
    temp_bomba = *bomba;
    pthread_mutex_unlock(&mtx_bombe);

    while (TRUE) {
        pthread_mutex_lock(&mtx_bombe);
        temp_bomba.vite = bomba->vite;
        pthread_mutex_unlock(&mtx_bombe);
        temp_bomba.pos.x -= 1;

        if (temp_bomba.pos.x < 0 || temp_bomba.pos.y >= maxy || temp_bomba.pos.y < 0) {
            temp_bomba.vite = 0;
            pthread_mutex_lock(&mtx_bombe);
            *bomba = temp_bomba;
            pthread_mutex_unlock(&mtx_bombe);
            pthread_exit(NULL);
        } else {
            pthread_mutex_lock(&mtx_bombe);
            *bomba = temp_bomba;
            pthread_mutex_unlock(&mtx_bombe);
            temp_bomba.old_pos = temp_bomba.pos;
            usleep(velocita_missili); //regola velocità missili
        }
    }
}
