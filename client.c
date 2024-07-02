#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "grid.c"

/* Globálne premenné */
char userName[21];
int mojeID = 0;
int somNaTahu = 0;
int poslednyHod = 0;
int endGame = 0;
/*********************/

/**
 * Spracováva prijate dáta od servera
 *
 * @param buffer Smerník na reťazec, ktorý spracuje
 */
void dataRecv(char *buffer) {
    int kod = buffer[0];
    int edit;
    int moznosti[4];
    int dlzkaSpravy = strlen(buffer);
    int koniec;
    int hracSkoncil;
    int pocetHracov;

    switch (kod) { // 1. znak = kód správy a určuje čo táto informácia predstavuje
        case 1: // Broadcast od 1. hráča
        case 2: // Broadcast od 2. hráča
        case 3: // Broadcast od 3. hráča
        case 4: // Broadcast od 4. hráča
            printf("Player(%d): %s\n", buffer[0], buffer + 1);
            break;
        case 10 :
            hracSkoncil = buffer[strlen(buffer) - 1] - '0';
            vymazHraca(hracSkoncil);
            creating_board(mojeID);
            printf("%s\n",buffer);
            break;
        case 99 : // update hracej plochy
            edit = buffer[1];
            for (int i = 0; i < edit; ++i) {
                /*************************** DEBUG ***************************/
                printf("UPDATE-(%d)\n", i + 1);
                printf("hrac-%d\n", buffer[(i * 4) + 3] - 1);
                printf("figurka-%d\n", buffer[(i * 4) + 4] - 1);
                printf("typ pola-%d\n", buffer[(i * 4) + 5] - 1);
                printf("pozicia-%d\n", buffer[(i * 4) + 6] - 1);
                /*************************************************************/
                koniec = buffer[3];
                nastavitfigurku(buffer[(i * 4) + 3] - 1,
                                buffer[(i * 4) + 4] - 1,
                                buffer[(i * 4) + 5] - 1,
                                buffer[(i * 4) + 6] - 1);
            }
            creating_board(mojeID);

            if (edit == 2) {
                if (somNaTahu)
                    printf("Vyhodil si figúrku hráča Player(%d)\n",buffer[3]);
                else if (buffer[3] == (mojeID + 1))
                    printf("Player(%d) vyhodil tvoju figúrku\n",buffer[7]);
                else
                    printf("Player(%d) vyhodil figúrku hráča Player(%d)\n",buffer[7],buffer[3]);
            }

            if (koniec == 1) {
                if ((somNaTahu == 1) && (poslednyHod != 6)) {
                    printf("Tvoj ťah skočil\n");
                    somNaTahu = 0;
                }
            } else {
                if (somNaTahu == 1) {
                    printf("Skončil si!\n");
                    //endGame = 1;
                    somNaTahu = 0;
                } else {
                    if (edit == 1) {
                        printf("Hráč %d skončil\n", buffer[3]);
                    } else {
                        printf("Hráč %d skončil\n", buffer[7]);
                    }
                }
            }
            break;
        case 100 : // invite message - úvodna správa pri priojení do hry
            mojeID = buffer[1] - 1;
            setPlayer(mojeID);
            pocetHracov = buffer[2];
            for (int i = pocetHracov; i < 4; ++i) {
                vymazHraca(i);
            }
            creating_board(mojeID);
            printf("%s\n", buffer + 3);
            break;
        case 101 :
            printf("Hra skončila\n");
            endGame = 1;
            break;

        case 111 : // Chybná hláška pri nesprávnom vstupe
            printf("ERROR: %s\n", buffer + 1);
            break;
        case 122: // Informuje o hodoch ostatných hráčov
            printf("Hráč %d hodil(%d)\n", buffer[2], buffer[1]);
            break;
        case 124: // Informuje o tom, že tento hráč je na rade
            /*********************** natiahnutie dát z prijatej správy ***********************/
            somNaTahu = 1;
            moznosti[0] = buffer[dlzkaSpravy - 4]; // 0(A) - figúrka HODNOTY[1: áno | 2: nie]
            moznosti[1] = buffer[dlzkaSpravy - 3]; // 1(B) - figúrka HODNOTY[1: áno | 2: nie]
            moznosti[2] = buffer[dlzkaSpravy - 2]; // 2(C) - figúrka HODNOTY[1: áno | 2: nie]
            moznosti[3] = buffer[dlzkaSpravy - 1]; // 3(D) - figúrka HODNOTY[1: áno | 2: nie]
            buffer[dlzkaSpravy - 4] = 0; // vynuluje načítaný znak
            buffer[dlzkaSpravy - 3] = 0; // vynuluje načítaný znak
            buffer[dlzkaSpravy - 2] = 0; // vynuluje načítaný znak
            buffer[dlzkaSpravy - 1] = 0; // vynuluje načítaný znak
            poslednyHod = buffer[7] - '0'; // uloží si hodnotu hodu
            ///////////////////////////////////////////////////////////////////////////////////

            /************* Výpis možných pohybov s figúrkami *************/
            printf("%s \nVyber si jednu z figurok:", buffer + 1);
            for (int i = 0; i < 4; ++i) {
                if (moznosti[i] == 1) {
                    printf(" %d->%c", i, i + 'A');
                }
            }
            printf("\n");
            ///////////////////////////////////////////////////////////////
            break;

        case 125 : // Informuje o tom, že hráč svojim hodom nemôže ďalej nič robiť
            printf("%s\n", buffer + 1);
            break;
    }
}

/**
 * Zabezpečuje CLI pre hráča
 *
 * @param buffer Smerník na poľe do ktorého zapíše vystup z konzole a prípadne upraví
 * @return 1 ak užívateľ chce ukončiť spojenie
 */
int console(char *buffer) {
    bzero(buffer, 256); // Prečistenie buffra
    buffer[0] = mojeID + 1; // Označkovanie správy svojim id
    fgets(buffer + 1, 255, stdin); // Načítanie z konzole do buffra + 1 pretože 1 byte je označenie typu správy;

    /************************** Ukončenie programu **************************/
    if (strcmp(buffer, ":exit\n") == 0 || buffer[0] == 'x' || buffer[1] == 'x')
        return 1;
    //////////////////////////////////////////////////////////////////////////

    if (somNaTahu) {
        int move = buffer[1] - '0' + 1; // uloží si číslo figurky načítané zo vstupu v konzole
        bzero(buffer, 256); // vyčistí celý buffer
        buffer[0] = 66; // kód správy 66 informácia pre server
        buffer[1] = mojeID + 1; // id hráča
        buffer[2] = move; // číslo figúrky s ktorou chce hráč pohnúť
        buffer[3] = poslednyHod; // hod kockou, pre tento ťah
    } else {
        buffer[strlen(buffer) - 1] = 0; // vymaže {Enter}
    }
    return 0;
}

/**
 * Asynchrónna komunikácia medzi serverom a hráčom
 *
 * @param socket Socket descriptor na ktorom komunikuje so serverom
 * @return Vráti 0 keď sa ukončí spojenie
 */
int communication(int socket) {
    char buffer[256]; //Buffer
    bzero(buffer, 256); // nastav 0 bity v buffri

    fd_set sockets; // Asynchronna komunikácia
    FD_ZERO(&sockets); // Nastavenie 0 bitov v strukture fd_set
    int n, m; // návratové hodnoty pre write a read;

    while (1) {
        FD_SET(socket, &sockets); // vstup zo socketu
        FD_SET(STDIN_FILENO, &sockets); // vstup z konzole
        select(socket + 1, &sockets, NULL, NULL, NULL); // select

        if (FD_ISSET(socket, &sockets)) { // prípad ak má čo čítať
            bzero(buffer, 256); // Anulácia bitov v buffri
            n = read(socket, buffer, 255);
            printf("Kód(%d), Správa: %s\n", buffer[0], buffer + 1);
            if (n < 0) {
                break;
            } else if (n == 0) {
                break;
            }
            if (endGame) {
                printf("Hra skončila\n");
                break;
            }

            dataRecv(buffer); // spracovanie správy od servera
        }

        if (FD_ISSET(STDIN_FILENO, &sockets)) { // zadaný vstup v konzole
            if (console(&buffer)) { // ovladanie vstupov konzoly je zabezpečene v metóde
                break; // ak chce používateľ ukončit aplikáciu vráti hodnotu 0 a ukončí tým cyklus
            }

            m = write(socket, buffer, strlen(buffer) + 1); // upravený buffer pošle na server
            if (m < 0) {
                break;
            } else if (m == 0) {
                break;
            }
        }
    }
    return 0;
}

/**
 * Zabezpečuje spustiteľnosť klientskej časťi hry pre hráča
 *
 * @param argc Počet vstupných argumentov
 * @param argv 1. Argument je adresa server | 2. Argument port | 3. Argument hráčov nickname (Volitelná možnosť)
 * @return
 */
int main(int argc, char *argv[]) {
    int sockfd; //socket
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc == 4)
        strcpy(userName, argv[3]); //nickname zadaný ako vstupný argument
    else {
        printf("Enter username (maximum 20 characters): ");
        fgets(userName, 20, stdin); // ak vstupný argument nie je zadaný načíta nickname z konzoly
    }

    if (argc < 3) { // Kontrola zadaného portu ako vstupný argument
        fprintf(stderr, "Usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) { // Kontrola dobre zadanej sieťovej adresy ako vstupný argument
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;//ipv4
    bcopy(
            (char *) server->h_addr,
            (char *) &serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 3;
    }
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to socket");
        return 4;
    }

    int exitNumber = communication(sockfd);
    close(sockfd);
    return exitNumber;
}
