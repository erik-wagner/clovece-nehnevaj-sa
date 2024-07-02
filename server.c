#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#define DEFAULT_PORT 10801 /* defaultné číslo portu */
#define QUEUE_LENGTH 10 /* veľkosť frontu */
#define NUMBER_OF_PLAYERS 2 /* počet hráčov */
#define BUFFER_SIZE 256 /* velkosť buffru */



typedef struct playerData {
    int socketD;
    int id;
    char *buffS;
    int zivy;
    pthread_mutex_t *mut;
    pthread_cond_t *posli;
    pthread_cond_t *pridaj;
} player_data;

typedef struct send {
    player_data *a;
    pthread_mutex_t *mut;
    pthread_cond_t *posli;
    pthread_cond_t *pridaj;
    char *buffS;
} SND;

typedef struct Figurka {
    int pozicia;
    int typ_pola;
    int hrac;
} FIGURKA;

/* Globálne premenné */
int turn = -1;
int hrajuci[NUMBER_OF_PLAYERS];
FIGURKA figurkyVhre[16];
int koniec = 0;
int connected = 0;
/*********************/

/**************** Sekcia ovládanie hry ****************/
/**
 * Predá ťah ďaľšiemu možnému hráčovi
 *
 * @return Vráti -1 ak ostáva iba jeden hráč alebo žiadny v inakšiom prípade vráti 0;
 */
int posunKolo() {
    int count = 0;
    for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
        if (hrajuci[i])
            count++;
    }

    if (count < 2)
        return -1;


    do {
        turn++;
        turn = turn % NUMBER_OF_PLAYERS;
    } while (hrajuci[turn] == 0);
    return 0;
}

/**
 * Generuje hod kockou
 *
 * @return Vráti náhodné číslo od 1 po 6
 */
int kocka() {
    return 1 + rand() % 6;
}

/**
 * Metóda skontroluje možnosť pohybu vybranej figúrky pri určitom hode;
 *
 * @param fig Smerník na figurku u ktorej chceme skontrolovať možnosť pohybu.
 * @param hodKocky Predstavuje o koľko polí sa má figúrka posunúť
 * @return Vráti -1 ak jej pohyb je nemožný, ak sa pri takomto hode môže pohnúť vráti 0;
 */
int skontrolujPohyb(FIGURKA *fig, int hodKocky) {
    int final;

    switch (fig->typ_pola) {
        case 0:
            if (hodKocky == 6) {
                for (int i = (fig->hrac * 4); i < ((fig->hrac * 4) + 4); i++) {
                    if (figurkyVhre[i].typ_pola == 1) {
                        return -1;
                    }
                }
                return 0;
            } else {
                return -1;
            }

        case 1:
            for (int i = (fig->hrac * 4); i < ((fig->hrac * 4) + 4); i++) {
                if (figurkyVhre[i].typ_pola == 2 && figurkyVhre[i].pozicia == ((hodKocky - 1) + (fig->hrac * 9)))
                    return -1;
            }
            return 0;

        case 2:
            final = (fig->pozicia + hodKocky);

            /* HRAC 1,2,3 */
            if (fig->hrac != 0) {
                /* Prípad kedy vchádza do finálneho domčeku */
                if ((fig->pozicia < (fig->hrac * 9)) && final > ((fig->hrac * 9) - 1)) {
                    final = final - (fig->hrac * 9);
                    if (final >= 0 && final < 4) {
                        for (int i = (fig->hrac * 4); i < ((fig->hrac * 4) + 4); i++) {
                            if (figurkyVhre[i].typ_pola == 3 && figurkyVhre[i].pozicia == final) {
                                return -1; // finálna pozícia je už obsadená
                            }
                        }
                        return 0; // na finalnej pozicii sa nenachadza ziadna figurka
                    } else {
                        return -1; // final je mimo rozsahu <0;3>
                    }

                    /* Prípad ak ostáva v poli */
                } else {
                    final = final % 36;
                    for (int i = (fig->hrac * 4); i < ((fig->hrac * 4) + 4); i++) {
                        if (figurkyVhre[i].typ_pola == 2 && figurkyVhre[i].pozicia == final) {
                            return -1; // na finalnej pozicii je figurka obsadena
                        }
                    }
                    return 0; // pozícia je voľná
                }
            }

                /* HRAC 0 */
            else {
                /* Prípad kedy vchádza do finálneho domčeku */
                if (final > 35) {
                    final = final - 36;
                    if (final >= 0 && final < 4) { // final je v rozsahu <0;3>
                        for (int i = (fig->hrac * 4); i < ((fig->hrac * 4) + 4); i++) {
                            if (figurkyVhre[i].typ_pola == 3 && figurkyVhre[i].pozicia == final) {
                                return -1; // finálna pozicia je už obsadená
                            }
                        }
                        return 0; // finálna pozicia je voľná
                    } else {
                        return -1; // final je mimo rozsahu <0;3>
                    }
                }
                    /* Prípad keď ostáva v poli */
                else {
                    for (int i = (fig->hrac * 4); i < ((fig->hrac * 4) + 4); i++) {
                        if (figurkyVhre[i].typ_pola == 2 && figurkyVhre[i].pozicia == final) {
                            return -1; // finálna pozicia je už obsadená
                        }
                    }
                    return 0; // finálna pozicia je voľná
                }
            }


        case 3 :
            final = fig->pozicia + hodKocky;
            if (final >= 0 && final < 4) {
                for (int i = (fig->hrac * 4); i < ((fig->hrac * 4) + 4); i++) {
                    if (figurkyVhre[i].typ_pola == 3 && figurkyVhre[i].pozicia == final) {
                        return -1; // finálna pozicia je už obsadená
                    }
                }
                return 0; // finálna pozícia je voľná
            } else {
                return -1; // final je mimo rozsahu <0;3>
            }
        default:
            return -1;
    }
}

/**
 * Metóda skontroluje vyhodenie figúrky z hracieho poľa.
 *
 * @param fig Smerník na figurku u ktorej chceme kontrolovať či nevyhodila inú figúrku.
 * @return Vráti hodnotu -1, keď nevyhodila žiadnu, v prípade ak vyhodila vráti index vyhodenej figúrky.
 */
int vyhod(FIGURKA *fig) {
    if (fig->typ_pola == 2) {
        for (int i = 0; i < 16; ++i) {
            if ((&figurkyVhre[i] != fig) && (figurkyVhre[i].typ_pola == 2) && figurkyVhre[i].pozicia == fig->pozicia) {
                figurkyVhre[i].typ_pola = 0;
                figurkyVhre[i].pozicia = i % 4;
                return i;
            }
        }
    }
    return -1;
}

/**
 * Posunie figúrku v hraciom poli na základe hodu kockou
 *
 * @param fig Smerník na figúrku s ktorou sa chceme pohnúť.
 * @param hodKocky Číslo o koľko políčok sa má figúrka pohnúť
 */
void posunFigurku(FIGURKA *fig, int hodKocky) {  //posunie figurku o 1 dopredu
    int final;

    switch (fig->typ_pola) {
        case 0: //OK
            fig->typ_pola = 1;
            fig->pozicia = fig->hrac;
            break;

        case 1: //OK
            fig->typ_pola = 2;
            fig->pozicia = (fig->hrac * 9) + (hodKocky - 1);
            break;

        case 2:
            final = (fig->pozicia + hodKocky);
            /* HRAC 1,2,3 */
            if (fig->hrac != 0) {
                /* Prípad kedy vchádza do finálneho domčeku */
                if ((fig->pozicia < (fig->hrac * 9)) && final > ((fig->hrac * 9) - 1)) {
                    final = final - (fig->hrac * 9);
                    fig->typ_pola = 3;
                    fig->pozicia = final;

                    /* Prípad ak ostáva v poli */
                } else {
                    final = final % 36;
                    fig->pozicia = final;
                }
            }

                /* HRAC 0 */
            else {
                /* Prípad kedy vchádza do finálneho domčeku */
                if (final > 35) {
                    final = final - 36;
                    fig->typ_pola = 3;
                    fig->pozicia = final;
                }
                    /* Prípad keď ostáva v poli */
                else {
                    fig->pozicia = final;
                }
            }
            break;


        case 3 :
            fig->pozicia = fig->pozicia + hodKocky;
            break;
        default:
            break;
    }
}

/**
 * Skontroluje či už hráč vykonal svoj finálny krok
 *
 * @param hrac Index hráča, u ktorého chceme skontrolovať jeho koniec
 * @return Hráč skončil keď všetky jeho figúrky sa nachádzajú v hracom poli vtedi vráti 1, ináč vráti 0
 */
int hracSkoncil(int hrac) {
    for (int i = (hrac * 4); i < ((hrac * 4) + 4); i++) {
        if (figurkyVhre[i].typ_pola != 3) {
            return 0;
        }
    }
    return 1;
}
////////////////////////////////////////////////////////

/***************** Sekcia komunikácia *****************/

/**
 * Spracuje dáta a upraví ich podľa potreby
 *
 * @param buffer Smerník na reťazec s ktorým bude pracovať
 * @return Vráťi -1 ak dáta ešte potrebujú spracovanie
 */
int dataRecieve(char *buffer) {
    int hrac, cisloFig, hodKockou, idSpravy, vyhodenaFigurka, skoncil;

    /* Informácia priamo od hráča o jeho ťahu */
    idSpravy = buffer[0];
    if (idSpravy == 66) { //PLAYER MOVE
        /* NAČÍTA VŠETKY POTREBNÉ ÚDAJE O HRÁČOVOM KROKU */
        hrac = buffer[1] - 1; //vzdy spravne
        cisloFig = buffer[2] - 1; //kontrola
        hodKockou = buffer[3]; //vzdy spravne
        FIGURKA *f = &figurkyVhre[(hrac * 4) + cisloFig];
        //printf("PLAYER MOVE! {%d[hrac 1 - 4],%d[Figurka 1 - 4],%d[Hod kocky 1 - 6]} \n", buffer[1], buffer[2],buffer[3]);



        /* NESPRÁVNU VÝBER */
        int pohyb = skontrolujPohyb(f, hodKockou);
        //printf("pohyb %d\n",pohyb);

        if ((cisloFig < 0 || cisloFig > 3) || (pohyb == -1)) {
            bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, "*Nesprávna voľba figúrky, voľbu opakujte prosím znovu");
            buffer[0] = 111;
            buffer[strlen(buffer)] = hrac + 1;
            return 0;
        }

        posunFigurku(f, hodKockou);
        vyhodenaFigurka = vyhod(f);
        skoncil = hracSkoncil(hrac);
        if (skoncil) hrajuci[hrac] = 0;



        /************ UPDATE ************/
        bzero(buffer, BUFFER_SIZE);
        if (vyhodenaFigurka == -1) {
            buffer[0] = 99;
            buffer[1] = 1; //vyhodil hraca? 1 nie 2 ano
            buffer[2] = skoncil + 1; //hráč skončil? 1 nie 2 áno
            buffer[3] = hrac + 1;
            buffer[4] = cisloFig + 1;
            buffer[5] = f->typ_pola + 1;
            buffer[6] = f->pozicia + 1;
            buffer[7] = 5; //broadcast
            //printf("UPDATE! Posunutý{%d[hrac 1 - 4],%d[Figurka 1 - 4],%d[Typ pola 1 - 4],%d[Pozicia (zaleží od poľa))]} \n",buffer[3], buffer[4], buffer[5], buffer[6]);
        } else {
            bzero(buffer, BUFFER_SIZE);
            buffer[0] = 99; //Kód správy pre klienta
            buffer[1] = 2; // Vyhodil hráča? 1-nie 2-áno
            buffer[2] = skoncil + 1; // Hráč skončil? 1-nie 2-áno

            buffer[3] = figurkyVhre[vyhodenaFigurka].hrac + 1;
            buffer[4] = (vyhodenaFigurka % 4) + 1;
            buffer[5] = figurkyVhre[vyhodenaFigurka].typ_pola + 1;
            buffer[6] = figurkyVhre[vyhodenaFigurka].pozicia + 1;

            buffer[7] = hrac + 1;
            buffer[8] = cisloFig + 1;
            buffer[9] = f->typ_pola + 1;
            buffer[10] = f->pozicia + 1;

            buffer[11] = 5; //broadcast
            /*printf("UPDATE! Vyhodený{%d[hrac 1 - 4],%d[Figurka 1 - 4],%d[Typ pola 1 - 4],%d[Pozicia (zaleží od poľa))]} \n"
                   "Posunutý{%d[hrac 1 - 4],%d[Figurka 1 - 4],%d[Typ pola 1 - 4],%d[Pozicia (zaleží od poľa))]}\n",
                   buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10]);*/

        }
        if (hodKockou != 6) { //ak nepadla 6 tak je na rade ďaľší hráč
            posunKolo();
        }
        return -1;
    }
    /******************************************/

    /* Predošlá správa bola update */
    if (idSpravy == 99) {
        /*int count = 0;
        for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
            if (hrajuci[i])
                count++;
        }
        if (count < 2) {
            buffer[0] = 101;
            return 0;//
        }*/

        hrac = turn;
        hodKockou = kocka();
        bzero(buffer, BUFFER_SIZE);
        strcpy(buffer, "*Padla * si na rade");
        buffer[0] = 124;
        buffer[strlen(buffer)] = hrac + 1;


        for (int i = hrac * 4; i < (hrac * 4) + 4; i++) {
            if (skontrolujPohyb(&figurkyVhre[i], hodKockou) == 0) {
                buffer[strlen(buffer)] = 1; // ANO
            } else {
                buffer[strlen(buffer)] = 2; // NIE
            }
        }

        int dlzka = strlen(buffer);
        for (int i = dlzka - 4; i < dlzka; ++i) {
            if (buffer[i] == 1)
                break;

            if (i == (dlzka - 1) && buffer[i] == 2) {
                bzero(buffer, BUFFER_SIZE);
                strcpy(buffer, "*Padla *, bohužial nemáš možnosť pohybu a tvoj ťah končí");
                buffer[0] = 125;
            }
        }
        buffer[7] = hodKockou + '0';
        buffer[strlen(buffer)] = hrac + 1;
        if (buffer[0] == 125) {
            posunKolo();
            return -1;
        }
        return 0;
    }
    /*******************************/

    /* Predošlá správa niesla informáciu o hode, ktorým nemohol vykonať ťah */
    if (idSpravy == 125) {
        hrac = turn;
        hodKockou = kocka();
        bzero(buffer, BUFFER_SIZE);
        strcpy(buffer, "*Padla * si na rade");
        buffer[0] = 124;

        for (int i = hrac * 4; i < (hrac * 4) + 4; i++) {
            if (skontrolujPohyb(&figurkyVhre[i], hodKockou) == 0) {
                buffer[strlen(buffer)] = 1; // ANO
            } else {
                buffer[strlen(buffer)] = 2; // NIE
            }
        }


        for (int i = strlen(buffer) - 4; i < strlen(buffer); ++i) {
            if (buffer[i] == 1)
                break;

            if (i == (strlen(buffer) - 1) && buffer[i] == 2) {
                strcpy(buffer, "*Padla *, bohužial nemáš možnosť pohybu a tvoj ťah končí");
                buffer[0] = 125;
                buffer[strlen(buffer)] = turn;
            }
        }
        buffer[7] = hodKockou + '0';
        buffer[strlen(buffer)] = hrac + 1;
        if (buffer[0] == 125) {
            posunKolo();
            return -1;
        }
        return 0;
    }
    /************************************************************************/
    return 0;
}

/**
 * Vlákno slúží najmä pre odosielanie dát hráčom
 *
 * @param param dáta potrebne pre funkciu tohoto vlákna (struct send)
 * @return NULL keď skončí
 */
void *sendData(void *param) {
    printf("Vlákno sendData bolo spustené\n");
    SND *data = (SND *) param;
    int n;
    int end = 1;

    while (koniec == 0) {
        pthread_mutex_lock(data->mut);
        if (strlen(data->buffS) == 0) { // žiadna poziadavka
            pthread_cond_wait(data->pridaj, data->mut); // čaká na požiadavku
            if (koniec) {
                //pthread_exit(0);
                return NULL;
            }
        }
        /************************* Ukončenie hry *************************/
        if (data->buffS[0] == 101) {
            koniec = 1;
            for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
                if (data->a[i].zivy) {
                    n = write(data->a[i].socketD, data->buffS, strlen(data->buffS));
                    if (n < 0) {
                        perror("Error writing to socket");
                    }
                }
            }


            bzero(data->buffS, BUFFER_SIZE);
            pthread_mutex_unlock(data->mut);
            pthread_cond_signal(data->posli);
            sleep(3);
            return NULL;
            //pthread_exit(0);
            /*****************************************************************/

            /************************** Odpojenie hráča *******************************/
        } else if (data->buffS[0] == 10) {
            int endID = data->buffS[strlen(data->buffS) - 1] - '0';

            for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
                if (endID != i && data->a[i].zivy) {
                    n = write(data->a[i].socketD, data->buffS, strlen(data->buffS));
                    if (n < 0) {
                        perror("Error writing to socket");
                    }
                }
            }
            int ok = 1;
            for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
                if (data->a[i].zivy) {
                    ok = 0;
                    break;
                }
            }
            if (ok) end = 0;
            bzero(data->buffS, BUFFER_SIZE);
            pthread_cond_signal(data->posli);
            pthread_mutex_unlock(data->mut);
            /**************************************************************************/


            /**************************** Odoslanie hráčovi + (info ostatným) ****************************/
        } else {
            int id = data->buffS[strlen(data->buffS) - 1] - 1; //ID prijímateľa
            data->buffS[strlen(data->buffS) - 1] = 0; // Vynulovanie posledného znaku (informácia o prijímateľovi)

            /************************* Jeden konkrétny používateľ *************************/
            if (id != 4) {
                n = write(data->a[id].socketD, data->buffS, strlen(data->buffS));
                printf("sentData: n:(%d) ID cieľa :(%d) | Kód správy (%d) | Správa: %s \n", n, id, data->buffS[0],
                       data->buffS + 1); //DEBUG
                if (n < 0) {
                    perror("Error writing to socket");
                }
                /******************************************************************************/

                /************************ Info o hode hráča + informuje ostatných ************************/
                if (data->buffS[0] == 124 || data->buffS[0] == 125) { // o hode kocky informuje ostatných hráčov
                    int kcka = data->buffS[7] - '0';
                    bzero(data->buffS, BUFFER_SIZE);
                    data->buffS[0] = 122;
                    data->buffS[1] = kcka;
                    data->buffS[2] = id + 1;
                    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
                        if (data->a[i].zivy && i != id) {
                            printf("sentData: n:(%d) ID cieľa :(%d) | Kód správy (%d) | Správa: %s \n", n, id,
                                   data->buffS[0], data->buffS + 1); //DEBUG
                            n = write(data->a[i].socketD, data->buffS, strlen(data->buffS));

                            if (n < 0) {
                                perror("Error writing to socket");
                            }
                        }
                    }
                    bzero(data->buffS, BUFFER_SIZE);
                }
                /*********************************************************************************************/


                /***************************************** Broadcast *****************************************/
            } else {
                for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
                    if (data->a[i].zivy && i != (data->buffS[0] - 1)) {
                        n = write(data->a[i].socketD, data->buffS, strlen(data->buffS));
                        printf("sentData: n:(%d) ID cieľa :(%d) | Kód správy (%d) | Správa: %s \n", n, id, data->buffS[0], data->buffS + 1); // DEBUG
                        if (n < 0) {
                            perror("Error writing to socket");
                        }
                    }
                }
            }
            bzero(data->buffS, BUFFER_SIZE);
            pthread_mutex_unlock(data->mut);
            pthread_cond_signal(data->posli);
            /*********************************************************************************************/
        }
        usleep(100);
    }
    printf("Vlákno sendData bolo ukončené\n");
    return NULL;
}

/**
 * Vlákno prijíma dáta od konkrétneho používateľa
 *
 * @param param dáta hráča (struct player_data)
 * @return
 */
void *playerConnection(void *param) {
    player_data *data = (player_data *) param; // dáta hráča s ktorým vlákno komunikuje
    char buffer[BUFFER_SIZE]; // Buffer na spracovanie požiadavky a odoslanie
    int n; // návratová hodnota read()


    printf("Vlákno recvData(%d) bolo spustené\n", data->id);
    /************************** INVITE MESSAGE **************************/
    if (turn == -1) {
        strcpy(buffer, "***Čaká sa na pripojenie ostatných hráčov.");
    } else {
        strcpy(buffer, "***Všetci sa pripojili hra sa začína");
    }
    buffer[0] = 100;
    buffer[1] = data->id + 1;
    buffer[2] = NUMBER_OF_PLAYERS;
    buffer[strlen(buffer)] = data->id + 1;
    /********************************************************************/

    /********************** Prekopíruj na odoslanie **********************/
    pthread_mutex_lock(data->mut);
    if (strlen(data->buffS) != 0) {
        pthread_cond_wait(data->posli, data->mut);
        if (koniec){
            //pthread_exit(0);
            return NULL;
        }
    }
    strcpy(data->buffS, buffer);

    pthread_cond_signal(data->pridaj);
    pthread_mutex_unlock(data->mut);
    /*********************************************************************/

    int opakovat;
    int end = 1;

    /* Cyklus pre odpoveďe na požiadavky hráča */
    while (koniec == 0) {
        bzero(buffer, BUFFER_SIZE);
        n = read(data->socketD, buffer, BUFFER_SIZE);
        if (koniec) {
            //pthread_exit(0);
            return NULL;
        }
        /******************************* Ošetrenie odpojenia hráča *******************************/
        if (n <= 0) {
            if (turn == -1) {
                strcpy(buffer, "*Prerušené spojenie s hráčom Player*");
                buffer[0] = 10;
                hrajuci[data->id] = 0;
                data->zivy = 0;
                /*********** Kritická sekcia -> úprava buffru na odoslanie ************/

                pthread_mutex_lock(data->mut);
                if (strlen(data->buffS) != 0) {
                    pthread_cond_wait(data->posli, data->mut);
                    if (koniec) {
                        //pthread_exit(0);
                        return NULL;
                    }
                }
                strcpy(data->buffS, buffer);
                pthread_cond_signal(data->pridaj);
                pthread_mutex_unlock(data->mut);
                /**********************************************************************/
                break;
            }

            if (koniec) {
                break;
            }
            data->zivy = 0;
            hrajuci[data->id] = 0;
            if (data->id == turn) {
                if (posunKolo() == -1) {
                    bzero(buffer, BUFFER_SIZE);
                    strcpy(buffer, "*Prerušené spojenie s hráčom Player*");
                    buffer[0] = 10;
                    /*********** Kritická sekcia -> úprava buffru na odoslanie ************/
                    pthread_mutex_lock(data->mut);
                    if (strlen(data->buffS) != 0) {
                        pthread_cond_wait(data->posli, data->mut);
                        if (koniec) {
                            //pthread_exit(0);
                        }
                    }
                    strcpy(data->buffS, buffer);
                    pthread_cond_signal(data->pridaj);
                    pthread_mutex_unlock(data->mut);
                    /**********************************************************************/
                    usleep(50);
                    buffer[0] = 101;
                    /*********** Kritická sekcia -> úprava buffru na odoslanie ************/
                    pthread_mutex_lock(data->mut);
                    if (strlen(data->buffS) != 0) {
                        pthread_cond_wait(data->posli, data->mut);
                        if (koniec) {
                            //pthread_exit(0);
                            return NULL;
                        }
                    }
                    strcpy(data->buffS, buffer);
                    pthread_cond_signal(data->pridaj);
                    pthread_mutex_unlock(data->mut);
                    /**********************************************************************/
                    break;
                } else {
                    strcpy(buffer, "*Prerušené spojenie s hráčom Player*");
                    buffer[0] = 10;
                    buffer[strlen(buffer) - 1] = data->id + '0';
                    /*********** Kritická sekcia -> úprava buffru na odoslanie ************/
                    pthread_mutex_lock(data->mut);
                    if (strlen(data->buffS) != 0) {
                        pthread_cond_wait(data->posli, data->mut);
                        if (koniec) {
                            //pthread_exit(0);
                            return NULL;
                        }
                    }
                    strcpy(data->buffS,buffer);//
                    pthread_mutex_unlock(data->mut);
                    pthread_cond_signal(data->pridaj);
                    /**********************************************************************/
                    int repeat;
                    do {
                        bzero(buffer, BUFFER_SIZE);
                        buffer[0] = 99;
                        buffer[1] = 1;
                        buffer[2] = turn;
                        repeat = dataRecieve(buffer);
                        /*********** Kritická sekcia -> úprava buffru na odoslanie ************/
                        pthread_mutex_lock(data->mut);
                        if (strlen(data->buffS) != 0) {
                            pthread_cond_wait(data->posli, data->mut);
                            if (koniec) {
                                //pthread_exit(0);
                                return NULL;
                            }
                        }
                        strcpy(data->buffS,buffer);//
                        pthread_mutex_unlock(data->mut);
                        pthread_cond_signal(data->pridaj);
                        /**********************************************************************/
                        sleep(1);
                    } while (repeat == -1);
                    printf("Player(%d) has disconnected\n", data->id);
                    return NULL;
                }
            } else {
                int count = 0;
                for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
                    if (hrajuci[i] == 1)
                        count++;
                }
                if (count < 2) {
                    buffer[0] = 101;
                    /************** Kritická sekcia -> úprava buffru na odoslanie ***************/
                    pthread_mutex_lock(data->mut);
                    if (strlen(data->buffS) != 0) {
                        pthread_cond_wait(data->posli, data->mut);
                        if (koniec) {
                            //pthread_exit(0);
                            return NULL;
                        }
                    }
                    strcpy(data->buffS, buffer);
                    pthread_mutex_unlock(data->mut);
                    pthread_cond_signal(data->pridaj);
                    /****************************************************************************/
                    break;
                }
                //

                strcpy(buffer, "*Prerušené spojenie s hráčom Player*");
                buffer[0] = 10;
                buffer[strlen(buffer) - 1] = data->id + '0';

                end = 0;
                printf("Player(%d) has disconnected\n", data->id);
                return NULL;
            }
            /*****************************************************************************************/


            /********************************* Požiadavka zo strany hráča ********************************/
        } else {
            printf("recvData: Kód správy(%d) Správa: %s \n", buffer[0], buffer + 1);
        }
        do { // opakuje kým požiadavka nie je kompletne spracovaná
            opakovat = 0;
            /* Broadcast len označkuje a pošle ostatným */
            if (buffer[0] >= 1 && buffer[0] <= 4) { // Podmienka pre zistenie či správa má byť ako broadcast do chatu
                buffer[strlen(buffer)] = 5; // informácia o broadcaste pre všetkých online účastníkov
            }
                /* (66) s tým, že hráč nie je na rade */
            else if (buffer[0] == 66 && turn != data->id) { // kód(66) predstavuje informáciu o ťahu hráča
                strcpy(buffer, "*Momentálne niesi na rade.");
                buffer[0] = 111;//kód(111) správa pre klienta že jeho ťah je neplatný, pretože nie je na rade
                buffer[strlen(buffer)] = data->id + 1;// informácia, ktorý hráč má dostať túto správu
            } else if (buffer[0] == 10) {
                end = 0;
            } else {
                opakovat = dataRecieve(buffer); // Požiadavku spracováva metóda
            }

            /******************* Kritická sekcia -> úprava buffru na odoslanie *******************/
            pthread_mutex_lock(data->mut);
            if (strlen(data->buffS) != 0) {
                pthread_cond_wait(data->posli, data->mut);
                if (koniec) {
                    //pthread_exit(0);
                    return NULL;
                }
            }
            strcpy(data->buffS, buffer);
            pthread_cond_signal(data->pridaj);
            pthread_mutex_unlock(data->mut);
            /*************************************************************************************/
            sleep(1);
        } while (opakovat == -1);
    }
    /*********************************************************************************************/

    close(data->socketD);
    printf("Vlakno recvData(%d) bolo ukončené\n", data->id);
    return NULL;
}

pthread_t sendThread;
pthread_t client_connection_threadID[NUMBER_OF_PLAYERS];
pthread_mutex_t mut_ex;
pthread_cond_t posli, pridaj;
int sockets[NUMBER_OF_PLAYERS];
int mastersck = -1;

void exitallthreads(int sign){
    koniec = 1;
    if (mastersck != -1)
        close(mastersck);
    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        if (sockets[i] != -1){
            close(sockets[i]);
        }
    }
    usleep (1000);
    pthread_mutex_destroy(&mut_ex);
    pthread_cond_destroy(&posli);
    pthread_cond_destroy(&pridaj);

    usleep (1000);
    pthread_cancel(sendThread);
    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        pthread_cancel(client_connection_threadID[i]);
    }
    pthread_detach(sendThread);
    for (int i = 0; i < connected; ++i) {
        pthread_detach(client_connection_threadID[i]);
    }
    sleep(1);
    exit(EXIT_SUCCESS);
}


/**
 * Main funkcia potrebná pre spustiteľnosť servera. Obsluhuje najmä sockety a vytvára spojenie medzi hráčmi
 *
 * @param argc počet vstupných argumentov
 * @param argv vstupné argumenty 1. argument by mal obsahovať číslo portu
 * @return 0 ak všetko dobre dopadlo
 */
int main(int argc, char *argv[]) {
    signal(SIGPIPE, exitallthreads);
    signal(SIGINT, exitallthreads);
    srand(time(NULL));
    char s[BUFFER_SIZE];
    player_data playerData[4];
    bzero(s, BUFFER_SIZE);

    struct sockaddr_in server_address, client_address; /* struktura pre serverovu a klientovu adresu */
    int master_socketd, new_client_socketd, port; //port servera
    socklen_t client_address_length;                /* dĺžka adresy */

    pthread_cond_init(&posli, NULL);
    pthread_cond_init(&pridaj, NULL);
    pthread_mutex_init(&mut_ex, NULL);

    for (int i = 0; i < 16; ++i) {
        figurkyVhre[i].hrac = i / 4;
        figurkyVhre[i].pozicia = i % 4;
        figurkyVhre[i].typ_pola = 0;
    }
    for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
        hrajuci[i] = 0;
        playerData[i].zivy = 0;
        sockets[i] = -1;
    }
    SND d = {playerData, &mut_ex, &posli, &pridaj, s};


    // Nastavenie štruktúry "server_address"
    memset((char *) &server_address, 0, sizeof(server_address)); /* nastavenie všetkých bitov na 0 */
    server_address.sin_family = AF_INET;            /* IPv4 protkol */
    server_address.sin_addr.s_addr = INADDR_ANY;    /* Nastavenie lokálnej ip adresy servera */


    if (argc > 1) { //Kontrola či je zadaný parameter pre špecifický port
        port = atoi(argv[1]); //Konvertuje 2. argument programu do celého čísla
    } else {
        port = DEFAULT_PORT; //V prípade ak nie je zadaný 2. argument použije sa defaultne prednastavený port
    }
    if (port > 0) { //kontrola nesprávneho portu
        if (port > 65535) //port nemôže presahovať maximálnu hranicu
            port = DEFAULT_PORT; //port v takomto prípade sa nastaví na defaultnu hodnotu
        server_address.sin_port = htons(
                (u_short) port); //Skonvertuje 16-bitové celé číslo z reprezentácie používanej lokálnym počítačom, na sieťovú reprezentáciu
    }

    // Vytvorenie deskrpitora socketu
    master_socketd = socket(PF_INET, SOCK_STREAM, 0);
    mastersck = master_socketd = socket(PF_INET, SOCK_STREAM, 0);
    if (master_socketd < 0) {
        perror("Error creating socket");
        return 1;
    }
    ///////////////////////////////////
    // Bindnutie lokalnej adresy k socketu
    if (bind(master_socketd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket address");
        return 2;
    }
    //////////////////////////////////////
    // Listen s velkostou fronty definovanou v konštante na začiatku kódu
    if (listen(master_socketd, QUEUE_LENGTH) < 0) {
        perror("Error listening");
        return 3;
    }
    /////////////////////////////////////////////////////////////////////
    client_address_length = sizeof(client_address);
    pthread_create(&sendThread, NULL, sendData, &d);
    //pthread_detach(sendThread);
    //sdsd
    printf("Server is running. Port: %d\n", port);
    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        //Vytvorenie spojenia server-client
        if ((new_client_socketd = accept(master_socketd, (struct sockaddr *) &client_address, &client_address_length)) <
            0) {
            perror("ERROR on accept");
            return 4;
        }
        connected++;

        /* Pridelenie dát hráčovi *///////////////////////////////////////////////////////////////////////
        playerData[i].id = i; //ID hráča
        playerData[i].socketD = new_client_socketd; //Socketdeskriptor pre konektivitu s pripojeným hráčom
        sockets[i] = new_client_socketd; //Socketdeskriptor pre konektivitu s pripojeným hráčom
        playerData[i].buffS = s; //Spoločný buffer pre odosielanie požiadaviek
        playerData[i].pridaj = &pridaj; //Vláknová podmienka pre [producentov] - vytváranie požiadavky na odoslanie
        playerData[i].posli = &posli; //Vláknova podmienka pre [konzumenta] - spracovanie aktuálnej požiadavky
        playerData[i].mut = &mut_ex; //Mutex pre kritickú sekciu (pridávanie požiadavky na odoslanie)
        playerData[i].zivy = 1; //informácia o tom či je daný hráč pripojený
        //////////////////////////////////////////////////////////////////////////////////////////////////
        hrajuci[i] = 1;

        if (i == NUMBER_OF_PLAYERS - 1)
            turn = 0;
        pthread_create(&client_connection_threadID[i], NULL, playerConnection,&playerData[i]); //Vytvorí vlákno pre každého hráča
        //pthread_detach(client_connection_threadID[i]); //Vytvorí vlákno pre každého hráča
    }


    /************** Inicializácia hry **************/
    sleep(1);
    int repeat;
    do {
        int count = 0;
        for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
            if (hrajuci[i]) {
                count++;
            }
        }
        pthread_mutex_lock(&mut_ex);
        if (strlen(s) != 0) {
            pthread_cond_wait(&posli, &mut_ex);
            if (koniec) {
                //pthread_exit(0);
                return NULL;
            }
        }
        if (count > 1) {
            s[0] = 99;
            s[1] = 1;
            s[2] = turn;
            repeat = dataRecieve(s);
        } else {
            s[0] = 101;
            repeat = 0;
        }
        pthread_mutex_unlock(&mut_ex);
        pthread_cond_signal(&pridaj);
        sleep(1);
    } while (repeat == -1);
    /***********************************************/
    while (koniec==0) {
        sleep(1);
    }

    printf("All threads have ended. Server will exit\n");

    koniec = 1;
    if (mastersck != -1)
        close(mastersck);
    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        if (sockets[i] != -1){
            close(sockets[i]);
        }
    }

    for (int i = 0; i < NUMBER_OF_PLAYERS; ++i) {
        pthread_join(client_connection_threadID[i],NULL);
    }
    pthread_join(sendThread,NULL);
    pthread_mutex_destroy(&mut_ex);
    pthread_cond_destroy(&posli);
    pthread_cond_destroy(&pridaj);
    sleep(1);
    return 0;
}
