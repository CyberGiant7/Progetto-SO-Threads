#include "menu.h"

#define KEY_SPACE 32
#define DIM_NAVICELLA 6
#define DIM_NEMICO 3
#define NAVE 0  // identificatore posizione navicella
#define MISSILE_1 1 // identificatore posizione Missile 1
#define MISSILE_2 2 // identificatore posizione Missile 2
#define MAX_MISSILI 10 // Numero (pari) massimo di missili sparabili contemporaneamente




const Oggetto init = {0, 0, {0, 0}, {0, 0}, 0, 0}; //Costante utilizzata per inizializzare la struttura OGGETTO
void* nave_player(void* arg);
_Bool AreaGioco(Oggetto *navicella, Oggetto *enemies, arg_to_navicella *argToNavicella);
void Enemy(int pipein, int pipeout, Oggetto enemy);
void* missili(void* arg);



void collisione_missili_bombe(Oggetto *missili, Oggetto *bombe_nem, int pipe_to_navicella,int *pipe_to_nemici);
void collisione_bombe_navicella(Oggetto *navicella, Oggetto *bombe_nem, int *pipe_to_nemici);
void collisione_missili_nemici(Oggetto *enemies, Oggetto *missili, int pipe_to_navicella, int *pipe_to_nemici, InfoToNemici *infoToNemici);

int M = 24; //Numero nemici
int num_righe = 6; //numero di nemici in ogni colonna
int mov_verticale = 6;
int velocita_missili = 100000;




char *nave[DIM_NAVICELLA]= {" ▟█▛▀▀",
                            "▟██▙  ",
                            "▗▟█▒▙▖",
                            "▝▜█▒▛▘",
                            "▜██▛  ",
                            " ▜█▙▄▄"
};

char *nemico_lv1[DIM_NEMICO]={"▀█▙",
                              "▒█ ",
                              "▄█▛"
};

char *nemico_lv2[DIM_NEMICO]={" △ ",
                              "◁ ◊",
                              " ▽ "
};
char *nemico_lv3[DIM_NEMICO]= {" ☠ ",
                               "☠ ☠", ///○ ◙ ❍ ◚
                               " ☠ "};

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

extern int maxx;
extern int maxy;

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


StatoCorrente gioco() {

    int i, l, n;
    pthread_t t_navicella;

    Oggetto navicella = {0, ID_NAVICELLA, {1, maxy/2}, {-1, -1}, t_navicella, 3};
    arg_to_navicella argToNavicella = {&navicella, FALSE};
    _Bool WIN;
    l = 0, n = 0;
    Oggetto *enemies = NULL;
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
    pthread_create(&t_navicella, NULL, &nave_player, &argToNavicella);
//     thread nemici
//     thread missili
//     thread bombe

//    nave player

//    nave_player(fd1[1], fd2[0]); /* il secondo processo figlio invoca la funzione nave_player passandogli la pipe in scrittura*/
//    nemici
//    enemies = (Oggetto *) calloc(M, sizeof(Oggetto));
//    for (i = 0; i < M; i++) {
//        enemies[i].index = i;
//        enemies[i].id = ID_NEMICO;
//    }

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
    return MENU;
}

//void Enemy(int pipein, int pipeout, Oggetto enemy) {
//    int j = 0, l = 0, temp, k;
//    int status = 0;
//    char nome[10];
//    int random;
//    int fd1[2];
//    _Bool spara;
//    pid_t pid_bomba;
//    Oggetto bomba, bomba_temp;
//    InfoToNemici info;
//    sprintf(nome, "Nemico_%d", enemy.index);
//    prctl(PR_SET_NAME, (unsigned long) nome);
//    k = 17;//3 quasi ok
//    prctl(PR_SET_PDEATHSIG, SIGCHLD);
//    bomba = init;
//    bomba.id = ID_BOMBA;
//
//    enemy.vite = 3;
//    enemy.pid = getpid();
//    enemy.old_pos.x = -1;
//    enemy.old_pos.y = -1;
//    info.vite = 3;
//    info.primafila = FALSE;
//    if (pipe(fd1) == -1) { //bombe -> nemici
//        perror("Errore nella creazione della pipe!");
//        _exit(1);
//    }
//    if (fcntl(fd1[0], F_SETFL, O_NONBLOCK) < 0) //pipe non bloccante
//        exit(2);
//    if (fcntl(fd1[1], F_SETFL, O_NONBLOCK) < 0) //pipe non bloccante
//        exit(2);
//
//
//    while (true) {
//        enemy.pos.y = (1 + (enemy.index % num_righe) * (DIM_NEMICO + 1)) + j;
//        temp = (int) (enemy.index / num_righe) + 1;
//        enemy.pos.x = maxx - temp * (DIM_NEMICO + 1) - l;
//        enemy.id = ID_NEMICO;
//        bomba.pos.x = enemy.pos.x - 2;
//        bomba.pos.y = enemy.pos.y + 1;
//        bomba_temp.vite = 0;
//        int signal;
//
//        if (enemy.vite == 0) {
//            write(pipeout, &enemy, sizeof(Oggetto));
//            wait(&signal);
//            exit(1);
//        } else {
//            write(pipeout, &enemy, sizeof(Oggetto));
//        }
//        read(pipein, &info, sizeof(InfoToNemici));
//        enemy.vite = info.vite;
//
//
//        /*Spostamenti navicelle*/
//        if (j % 8 == 0) {
//            l++;
//        }
//
//        usleep(300000); //velocità spostamento nemici
//        if (j <= mov_verticale && status == 0) {
//            j++;
//            if (j == mov_verticale)
//                status = 1;
//        } else if (j >= 0 && status == 1) {
//            j--;
//            if (j == 0)
//                status = 0;
//        }
//
//        for (int i = 0; i < 100; ++i) {
//            read(fd1[0], &bomba_temp, sizeof(Oggetto));
//        }
//
//        if (bomba_temp.vite == 1) {
//            spara = false;
//        } else if (bomba_temp.pid != 0 && info.primafila == true) {
////            kill(bomba_temp.pid, SIGKILL);
//            spara = true;
//        }
//
//
//        /*generazione bombe*/
//        if (spara == TRUE) {
//            pid_bomba = fork(); //generazione processo
//            switch (pid_bomba) {
//                case -1:
//                    perror("Errore nell'esecuzione della fork.");
//                    exit(1);
//                case 0:
//                    sprintf(nome, "Bomber %d", enemy.index);
//                    prctl(PR_SET_NAME, (unsigned long) nome);
//                    close(fd1[0]);
//                    bomba.vite = 1;
//                    bomba.index = enemy.index;
//                    write(fd1[1], &bomba, sizeof(Oggetto));
//                    if (enemy.index % 4 == 0) {
//                        sleep(1);
//                    } else if (enemy.index % 2 == 1) {
//                        sleep(3);
//                    } else if (enemy.index % 2 == 0) {
//                        sleep(5);
//                    }
//                    missili(pipeout, fd1[1], 3, &bomba);
//                    break;
//                default: //processo padre
//                    break;
//            }
//        }
//        enemy.old_pos = enemy.pos;
//    }
//}


void* nave_player(void *arg) {
    int i = 0, j = 0;
    int num_missili = 0;
    arg_to_navicella *argToNavicella;
    argToNavicella = (arg_to_navicella *) arg;
    Oggetto *navicella = argToNavicella->navicella;
    _Bool *sparo = &argToNavicella->sparo;

    keypad(stdscr, TRUE);
    while (TRUE) {
//        nodelay(stdscr, 1);
//        timeout(500);
        int c = getch();
        switch (c) {
            case KEY_UP:
                pthread_mutex_lock(&mutex_nave);
                if (navicella->pos.y > 1)
                    navicella->pos.y -= 1;
                pthread_mutex_unlock(&mutex_nave);
                break;
            case KEY_DOWN:
                pthread_mutex_lock(&mutex_nave);
                if (navicella->pos.y < maxy - DIM_NAVICELLA)
                    navicella->pos.y += 1;
                pthread_mutex_unlock(&mutex_nave);
                break;
            case KEY_SPACE:
                pthread_mutex_lock(&mutex_nave);
                *sparo = true;
                pthread_mutex_unlock(&mutex_nave);
                break;
        }
//        usleep()
    }
}
//if (num_missili <= MAX_MISSILI - 2) {
//                    num_missili += 2;
//                    missile1.pos.y = navicella.pos.y + (DIM_NAVICELLA / 2);
//                    missile1.pos.x = navicella.pos.x + DIM_NAVICELLA;
//                    missile2.pos.y = navicella.pos.y + (DIM_NAVICELLA / 2);
//                    missile2.pos.x = navicella.pos.x + DIM_NAVICELLA;
//
//                    missile1.id = ID_MISSILE;
//                    missile1.index = num_missili - 2;
//                    missile2.id = ID_MISSILE;
//                    missile2.index = num_missili - 1;
//
//                    i = 0;
//                    pid_missile1 = fork(); //generazione processo
//                    switch (pid_missile1) {
//                        case -1:
//                            perror("Errore nell'esecuzione della fork.");
//                            exit(1);
//                        case 0:
//                            prctl(PR_SET_NAME, (unsigned long) "Missile 1");
//                            close(filedes[0]);
//                            missili(pipeout, filedes[1], 1, &missile1);
//                            break;
//                        default: //processo padre
//                            pid_missile2 = fork(); //generazione di un secondo processo figlio per la nave_player
//                            switch (pid_missile2) {
//                                case -1:
//                                    perror("Errore nell'esecuzione della fork.");
//                                    exit(1);
//                                case 0:
//                                    prctl(PR_SET_NAME, (unsigned long) "Missile 2");
//                                    close(filedes[0]);
//                                    missili(pipeout, filedes[1], 2, &missile2);
//                                    break;
//                                default:
//                                    break;
//                            }
//                            break;
//                    }
//                }
//prima legge dai figli missili
//        for (i = 0; i <= MAX_MISSILI * 100; i++) {
//            read(filedes[0], &temp_missile, sizeof(temp_missile));
//            if ((temp_missile.index) >= 0 && (temp_missile.index) < MAX_MISSILI) {
//                array_missili[temp_missile.index] = temp_missile;
//            }
//        }
//        //poi legge dal padre area gioco
//        for (i = 0; i <= MAX_MISSILI * 100; i++) {
//            read(pipein, &temp_missile, sizeof(temp_missile));
//            if ((temp_missile.index) >= 0 && (temp_missile.index) < MAX_MISSILI) {
//                array_missili[temp_missile.index].vite = temp_missile.vite;
//            }
//        }
//        clear();
//        for (i = 0, j = 0; i < MAX_MISSILI; i++) {
//            if (array_missili[i].vite == 0) {
//                j++;
//            }
//            mvprintw(i+1,0,"array_missili[%d].vite= %d, j= %d", i,array_missili[i].vite, j);
//            refresh();
//}
//
//
//        if (num_missili == MAX_MISSILI && j == MAX_MISSILI) {
//            num_missili = 0;
//        }
//        clear();
//        mvprintw(1,0,"num_missili= %d, j= %d", num_missili, j);
//        refresh();


void* missili(void* arg) {
    Oggetto *missile, temp_missile;
    missile = (Oggetto*) arg;
    int i = 0;
    while (TRUE) {
        pthread_mutex_lock(&mutex_missili);
        temp_missile = *missile;
        pthread_mutex_unlock(&mutex_missili);
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
        if (temp_missile.id == ID_BOMBA) {
            temp_missile.pos.x -= 1;
        }
        i++;
        if ((temp_missile.pos.x > maxx || temp_missile.pos.y >= maxy || temp_missile.pos.y < 0) && (temp_missile.id == ID_MISSILE2 || temp_missile.id == ID_MISSILE1)) {
            temp_missile.vite = 0;
            pthread_mutex_lock(&mutex_missili);
            *missile = temp_missile;
            pthread_mutex_unlock(&mutex_missili);
            pthread_exit(NULL);
        } else if ((temp_missile.pos.x < 0 || temp_missile.pos.y >= maxy || temp_missile.pos.y < 0) && temp_missile.id == ID_BOMBA) {
            temp_missile.vite = 0;
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&mutex_missili);
        *missile = temp_missile;
        pthread_mutex_unlock(&mutex_missili);
        usleep(velocita_missili); //regola velocità missili
    }
}




/**
 * Funzione per gestire la stampa, le collisioni e la coordinazione di tutti i figli
 * @param pipein pipe per ricevere i valori dai figli
 * @param pipe_to_navicella pipe per inviare i valori alla navicella player
 * @param pipe_to_nemici
 * @param enemies
 * @return Il valore vittoria, vero se si vince e falso se si perde
 */
_Bool AreaGioco(Oggetto *navicella, Oggetto *enemies, arg_to_navicella *argToNavicella) {
    clear();
    refresh();
    int i, j, nemici_vivi, missili_vivi = 0, missili_morti = MAX_MISSILI;
    _Bool collision = false, WIN = false, sparo;
    Oggetto valore_letto = init;
    Oggetto rockets[MAX_MISSILI];
    Oggetto *bombe_nem = (Oggetto *) calloc(M, sizeof(Oggetto));
    InfoToNemici *message_to_nemici = (InfoToNemici *) calloc(M, sizeof(InfoToNemici));

    pthread_t t_missili[MAX_MISSILI];

//    for (i = 0; i < MAX_MISSILI; i++) {
//        rockets[i] = init;
//    }

//
//    for (i = 0; i < M; ++i) {
//        enemies[i].vite = 3;
//        message_to_nemici[i].vite = 3;
//        if (i >= (((M / num_righe) - 1) * num_righe) && i < M) {
//            message_to_nemici[i].primafila = TRUE;
//        } else {
//            message_to_nemici[i].primafila = FALSE;
//        }
//        write(pipe_to_nemici[i * 2 + 1], &message_to_nemici[i], sizeof(InfoToNemici));
//    }

    do {
        /// Lettura da tutti i processi figli
//        read(pipein, &valore_letto, sizeof(valore_letto)); /* leggo dalla pipe */
//
//        /// Stampa dei vari oggetti
//        switch (valore_letto.id) {
//            case ID_NAVICELLA:
//                stampaNavicella(valore_letto);
//                navicella->pos = valore_letto.pos;
//                navicella->old_pos = valore_letto.old_pos;
//                navicella->index = valore_letto.index;
//                navicella.pid = valore_letto.pid;
//                navicella->id = valore_letto.id;
//                break;
//            case ID_NEMICO:
//                stampaNemico(valore_letto, enemies[valore_letto.index].vite);
//                if (enemies[valore_letto.index].vite > 0) {
//                    enemies[valore_letto.index].pos = valore_letto.pos;
//                    enemies[valore_letto.index].old_pos = valore_letto.old_pos;
//                    enemies[valore_letto.index].index = valore_letto.index;
//                    enemies[valore_letto.index].pid = valore_letto.pid;
//                    enemies[valore_letto.index].id = valore_letto.id;
//                } else if (enemies[valore_letto.index].vite == 0) {
//                    enemies[valore_letto.index] = init;
//                }
//                break;
//            case ID_MISSILE:
//                stampaMiss_bomb(valore_letto);
//                rockets[valore_letto.index] = valore_letto;
//                break;
//            case ID_BOMBA:
//                stampaMiss_bomb(valore_letto);
//                bombe_nem[valore_letto.index] = valore_letto;
//                break;
//        }
//        /// Controllo delle varie collisioni
//        collisione_missili_bombe(rockets, bombe_nem, pipe_to_navicella, pipe_to_nemici);
//        collisione_bombe_navicella(&navicella, bombe_nem, pipe_to_nemici);
//        collisione_missili_nemici(enemies, rockets, pipe_to_navicella, pipe_to_nemici, message_to_nemici);
//
//        /// Collisione tra navicella e nemici
//        for (i = 0; i < M; i++) {
//            if (enemies[i].pos.x == DIM_NAVICELLA + 2) {
//                collision = TRUE;
//                WIN = FALSE;
//                break;
//            }
//        }
//        ///Controllo vite del player
//        if (navicella->vite == 0) {
//            WIN = FALSE;
//            collision = TRUE;
//        }
//        ///Controllo vite nemici
//        for (i = 0, j = 0; i < M; i++) {
//            if (enemies[i].vite == 0) {
//                j++;
//            }
//        }
//        nemici_vivi = M - j;
//        if (j == M) {
//            WIN = TRUE;
//            collision = true;
//        }
        }
        move(0, 0);
        clrtoeol();
        mvprintw(0, 0, "VITE: %d", navicella->vite);
        refresh();
    } while (!collision);
//    for (i = 0; i < M; ++i) {
//        if(enemies[i].pid != 0){
//            kill(enemies[i].pid, SIGKILL);
//        }
//    }
//    free(bombe_nem);
//    free(message_to_nemici);
    return WIN;
}

void stampaNavicella(Oggetto *navicella) {
    int i;

    pthread_mutex_lock(&mutex_nave);
    Oggetto temp_navicella = *navicella;
    navicella->old_pos = navicella->pos;
    pthread_mutex_unlock(&mutex_nave);
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
//void stampaNemico(Oggetto nemico, int vite){
//    int i;
//    if (nemico.pid != 0) {
//        attron(COLOR_PAIR(0));
//        for (i = 0; i < DIM_NEMICO; i++) {
//            mvprintw(nemico.old_pos.y + i, nemico.old_pos.x, "   ");
//        }
//        attroff(COLOR_PAIR(0));
//    }
//    switch (vite) {
//        case 3:
//            for (i = 0; i < DIM_NEMICO; i++) {
//                if (i == 1) {
//                    attron(COLOR_PAIR(4));
//                    mvprintw(nemico.pos.y + i, nemico.pos.x, nemico_lv1[i]);
//                    attroff(COLOR_PAIR(4));
//                } else {
//                    attron(COLOR_PAIR(7));
//                    mvprintw(nemico.pos.y + i, nemico.pos.x, nemico_lv1[i]);
//                    attroff(COLOR_PAIR(7));
//                }
//            }
//            break;
//        case 2:
//            attron(COLOR_PAIR(5));
//            for (i = 0; i < DIM_NEMICO; i++) {
//                mvprintw(nemico.pos.y + i, nemico.pos.x, nemico_lv2[i]);
//            }
//            attroff(COLOR_PAIR(1));
//            break;
//        case 1:
//            attron(COLOR_PAIR(5));
//            for (i = 0; i < DIM_NEMICO; i++) {
//                mvprintw(nemico.pos.y + i, nemico.pos.x, nemico_lv3[i]);
//            }
//            attroff(COLOR_PAIR(1));
//            break;
//        case 0:
//            attron(COLOR_PAIR(0));
//            for (i = 0; i < DIM_NEMICO; i++) {
//                mvprintw(nemico.pos.y + i, nemico.pos.x, "   ");
//            }
//            attroff(COLOR_PAIR(0));
//            break;
//    }
//}
void stampaMiss_bomb(Oggetto *bomb_miss) {
//    if (bomb_miss != 0) {
        attron(COLOR_PAIR(0));
        mvprintw(bomb_miss->old_pos.y, bomb_miss->old_pos.x, " ");
        attroff(COLOR_PAIR(0));
//    }
    if (bomb_miss->id == ID_MISSILE1 || bomb_miss->id == ID_MISSILE2) {
        attron(COLOR_PAIR(3));
        mvprintw(bomb_miss->pos.y, bomb_miss->pos.x, "⟢"); ///♿ ⟢ ⁂ ꗇ ꗈ 💣 🚀 卐 ◌́ ◌͂ ✝
        attroff(COLOR_PAIR(3));
    }
    else {
        attron(COLOR_PAIR(7));
        mvprintw(bomb_miss->pos.y, bomb_miss->pos.x, "⁂"); ///♿ ⟢ ⁂ ꗇ ꗈ 💣 🚀 卐 ◌́ ◌͂ ✝
        attroff(COLOR_PAIR(7));
    }
    bomb_miss->old_pos = bomb_miss->pos;
}

//void collisione_missili_bombe(Oggetto *missili, Oggetto *bombe_nem, int pipe_to_navicella,int *pipe_to_nemici){
//    int i, j;
//    /// collissione tra missili e bombe
//    for (i = 0; i < MAX_MISSILI; i++) {
//        if (missili[i].pid != 0) {
//            for (j = 0; j < M; j++) {
//                if (bombe_nem[j].pid != 0) {
//                    if (missili[i].pos.x == bombe_nem[j].pos.x && missili[i].pos.y == bombe_nem[j].pos.y){
//                        //uccido il missile colliso
//                        kill(missili[i].pid, SIGKILL);
//                        missili[i].vite = 0;
//                        write(pipe_to_navicella, &missili[i], sizeof(missili[i]));
//                        //uccido la bomba collisa
//                        kill(bombe_nem[j].pid, SIGKILL);
//                        bombe_nem[j].vite = 0;
//                        attron(COLOR_PAIR(0));
//                        mvaddstr(missili[i].old_pos.y, missili[i].old_pos.x, " ");
//                        mvaddstr(missili[i].pos.y, missili[i].pos.x, " ");
//                        mvaddstr(bombe_nem[j].pos.y, bombe_nem[j].pos.x, " ");
//                        attroff(COLOR_PAIR(0));
//                        missili[i] = init;
//                        bombe_nem[j] = init;
//                    }
//                }
//            }
//        }
//    }
//}
//void collisione_bombe_navicella(Oggetto *navicella, Oggetto *bombe_nem, int *pipe_to_nemici){
//    int i,j,k;
//    ///collisione tra bombe e navicella player
//    for (i = 0; i < M; i++) {
//        if (bombe_nem[i].pid != 0) {
//            for (j = 0; j < DIM_NAVICELLA; ++j) {
//                for (k = 0; k < DIM_NAVICELLA; ++k) {
//                    if (j > 0 && j<5){
//                        k = 5;
//                    }
//                    if(bombe_nem[i].pos.x == navicella->pos.x + k && bombe_nem[i].pos.y == navicella->pos.y + j){
////                          uccido la bomba collisa
//                        kill(bombe_nem[i].pid, SIGKILL);
//                        printf("\a");
//                        fflush(stdout);
//                        navicella->vite--;
//                        attron(COLOR_PAIR(0));
//                        mvaddstr(bombe_nem[i].pos.y, bombe_nem[i].pos.x, " ");
//                        attroff(COLOR_PAIR(0));
//                        bombe_nem[i] = init;
//                    }
//                }
//            }
//        }
//    }
//}
//void collisione_missili_nemici(Oggetto *enemies, Oggetto *missili, int pipe_to_navicella, int *pipe_to_nemici, InfoToNemici *infoToNemici) {
//    int i, j, k, l;
//    ///collisione tra Missili e Nemici
//    for (i = 0; i < M; i++) {
//        if (enemies[i].pid != 0) {
//            for (j = 0; j < MAX_MISSILI; j++) {
//                if (missili[j].pid != 0) {
//                    for (k = 0; k < DIM_NEMICO; ++k) {
//                        for (l = 0; l < DIM_NEMICO; ++l) {
//                            if (missili[j].pos.x == enemies[i].pos.x + l && missili[j].pos.y == enemies[i].pos.y + k) {
//                                kill(missili[j].pid, SIGKILL);
//                                missili[j].vite = 0;
//                                write(pipe_to_navicella, &missili[j], sizeof(missili[j]));
//                                enemies[i].vite -= 1;
//                                infoToNemici[i].vite -= 1;
//                                write(pipe_to_nemici[i * 2 + 1], &infoToNemici[i], sizeof(InfoToNemici));
//                                if ((i - num_righe) >= 0 && enemies[i].vite == 0){
//                                    infoToNemici[i-num_righe].primafila = TRUE;
//                                    write(pipe_to_nemici[(i-num_righe) * 2 + 1], &infoToNemici[i-num_righe], sizeof(InfoToNemici));
//                                }
//                                attron(COLOR_PAIR(0));
//                                mvaddstr(missili[j].pos.y, missili[j].pos.x, " ");
//                                attroff(COLOR_PAIR(0));
//                                missili[j] = init;
//                                if (enemies[i].vite == 0){
//                                    enemies[i] = init;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//}