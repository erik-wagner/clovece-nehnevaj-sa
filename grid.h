#define BLT "\x1B[34m"
#define BLK "\e[0;97m"
#define BLU "\e[44m"
#define RED "\e[41m"
#define GRN "\e[42m"
#define YEL "\e[43m"
#define GREY "\e[47m"
#define BOLDUND "\e[1m\e[4m"
#define WHT  "\e[107m"
#define DEFAULT_FONT_COLOR "\033[0m"
/*********************************** Globálne premenné ***********************************/
char field[36]={' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                ' ',' ',' ',' ',' ',' ',};
char nastav[4]={'x','x','x','x'};
char house[16] = {'!', '!', '!','!','$', '$', '$','$','*', '*', '*','*','&', '&', '&','&',};
char finish[16] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
int typPola[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int indexVPoli[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
/*****************************************************************************************/
/**
 * Prepíše hráčové figúrky na A, B, C, D
 *
 * @param hrac ID hráča, ktorému má prepísať figúrky na ABCD
 */
void setPlayer(int hrac);
/**
 * Vyfarbí grid
 *
 * @param x posun z pohľadu hráča. Posuvá sa podľa zvyškov po delení 4
 * @return Vráťi zvolenu farbu (Zelená,Červená,Modrá,Žltá)
 */
char vyfarbi(int x);
/**
 * Vykreslí hraciu plochu do konzoly
 *
 * @param hrac ID hráča ktorému sa má hracia plocha zobraziť z jeho pohľadu
 */
void creating_board(int hrac);
/**
 * Prehodí figúrku na základe parametrov
 *
 * @param hrac Vlastník figúrky
 * @param cislo_figurky Ktorá z jeho figúriek
 * @param typ_pola V ktorom poli sa bude nachádzať
 * @param index_v_poli index tohoto poľa
 */
void nastavitfigurku(int hrac, int cislo_figurky, int typ_pola, int index_v_poli);

#ifndef CLOVECE_GRID_H
#define CLOVECE_GRID_H

#endif //CLOVECE_GRID_H
