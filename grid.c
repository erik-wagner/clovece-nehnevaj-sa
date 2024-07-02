#include <stdio.h>
#include <term.h>
#include "grid.h"

void creating_board(int hrac) {
    clear_screen;
    clear_screen;
    printf(WHT BLT "\n\n\t"BOLDUND"Človeče, nehnevaj sa!\n\n"BLK WHT);
    printf(    "                                  "GREY"-------------------"WHT"\n");
    printf("          ");vyfarbi(hrac);printf("---------"WHT"               "GREY"| %c   | %c   | %c   |"WHT"              ",field[(16 + (9 * hrac)) % 36], field[(17 + (9 * hrac)) % 36], nastav[(2 + hrac) % 4]);vyfarbi(hrac+1);printf("---------"WHT"\n");
    printf("          ");vyfarbi(hrac);printf("| %c | %c |"WHT"               "GREY"|_____|_____|_____|"WHT"              ", house[(6 + (4 * hrac)) % 16], house[(7 + (4 * hrac)) % 16]);vyfarbi(hrac+1);printf("| %c | %c |"WHT"\n", house[(10 + (4 * hrac)) % 16], house[(11 + (4 * hrac)) % 16]);
    printf("          ");vyfarbi(hrac);printf("|-------|"WHT"               "GREY"| %c   ", field[(15 + (9 * hrac)) % 36]);vyfarbi(hrac+1);printf("| %c   |"GREY" %c   |"WHT"              ", finish[(8 + (4 * hrac)) % 16], field[(18 + (9 * hrac)) % 36]);vyfarbi(hrac+1);printf("|-------|"WHT"\n");
    printf("          ");vyfarbi(hrac);printf("| %c | %c |"WHT"               "GREY"|_____", house[(4 + (4 * hrac)) % 16], house[(5 + (4 * hrac)) % 16]);vyfarbi(hrac+1);printf("|_____|"GREY"_____|"WHT"              ");vyfarbi(hrac+1);printf("| %c | %c |"WHT"\n", house[(8 + (4 * hrac)) % 16], house[(9 + (4 * hrac)) % 16]);
    printf("          ");vyfarbi(hrac);printf("---------"WHT"               "GREY"| %c   ", field[(14 + (9 * hrac)) % 36]);vyfarbi(hrac+1);printf("| %c   |"GREY" %c   |"WHT"              ", finish[(9 + (4 * hrac)) % 16], field[(19 + (9 * hrac)) % 36]);vyfarbi(hrac+1);printf("---------"WHT"\n");
    printf("                                  "GREY"|_____");vyfarbi(hrac+1);printf("|_____|"GREY"_____|"WHT"\n");
    printf("                                  "GREY"| %c   ", field[(13 + (9 * hrac)) % 36]);vyfarbi(hrac+1);printf("| %c   |"GREY" %c   |"WHT"\n", finish[(10 + (4 * hrac)) % 16], field[(20 + (9 * hrac)) % 36]);
    printf("          "GREY" _______________________|_____");vyfarbi(hrac+1);printf("|_____|"GREY"_____|_______________________ "WHT"\n");
    printf("          "GREY"| %c   | %c   | %c   | %c   | %c   ", nastav[(1 + hrac) % 4], field[(9 + (9 * hrac)) % 36], field[(10 + (9 * hrac)) % 36], field[(11 + (9 * hrac)) % 36],field[(12 + (9 * hrac)) % 36]);vyfarbi(hrac+1);printf("| %c   |"GREY" %c   | %c   | %c   | %c   | %c   |"WHT"\n",
            finish[(11 + (4 * hrac)) % 16], field[(21 + (9 * hrac)) % 36], field[(22 + (9 * hrac)) % 36], field[(23 + (9 * hrac)) % 36], field[(24 + (9 * hrac)) % 36], field[(25 + (9 * hrac)) % 36]);
    printf("          "GREY"|_____|_____|_____|_____|_____");vyfarbi(hrac+1);printf("|_____|"GREY"_____|_____|_____|_____|_____|"WHT"\n");
    printf("          "GREY"| %c   ", field[(8 + (9 * hrac)) % 36]);vyfarbi(hrac);printf("| %c   | %c   | %c   | %c   |"GREY"     ", finish[(4 + (4 * hrac)) % 16], finish[(5 + (4 * hrac)) % 16], finish[(6 + (4 * hrac)) % 16], finish[(7 + (4 * hrac)) % 16]); vyfarbi(hrac+2);printf("| %c   | %c   | %c   | %c   |"GREY" %c   |"WHT"\n", finish[(15 + (4 * hrac)) % 16], finish[(14 + (4 * hrac)) % 16], finish[(13 + (4 * hrac)) % 16], finish[(12 + (4 * hrac)) % 16], field[(26 + (9 * hrac)) % 36]);
    printf("          "GREY"|_____"); vyfarbi(hrac);printf("|_____|_____|_____|_____|"GREY"_____"); vyfarbi(hrac+2);printf("|_____|_____|_____|_____|"GREY"_____|"WHT"\n");
    printf("          "GREY"| %c   | %c   | %c   | %c   | %c   ", field[(7 + (9 * hrac)) % 36], field[(6 + (9 * hrac)) % 36], field[(5 + (9 * hrac)) % 36], field[(4 + (9 * hrac)) % 36],field[(3 + (9 * hrac)) % 36]); vyfarbi(hrac+3);printf("| %c   |"GREY" %c   | %c   | %c   | %c   | %c   |"WHT"\n"
           , finish[(3 + (4 * hrac)) % 16], field[(30 + (9 * hrac)) % 36], field[(29 + (9 * hrac)) % 36], field[(28 + (9 * hrac)) % 36], field[(27 + (9 * hrac)) % 36], nastav[(3 + hrac) % 4]);
    printf("          "GREY"|_____|_____|_____|_____|_____"); vyfarbi(hrac+3);printf("|_____|"GREY"_____|_____|_____|_____|_____|"WHT"\n");
    printf("                                  "GREY"| %c   ", field[(2 + (9 * hrac)) % 36]); vyfarbi(hrac+3);printf("| %c   |"GREY" %c   |"WHT"                  \n", finish[(2 + (4 * hrac)) % 16], field[(31 + (9 * hrac)) % 36]);
    printf("                                  "GREY"|_____"); vyfarbi(hrac+3);printf("|_____|"GREY"_____|"WHT"\n");
    printf("          "); vyfarbi(hrac+3);printf("---------"WHT"               "GREY"| %c   ", field[(1 + (9 * hrac)) % 36]); vyfarbi(hrac+3);printf("| %c   |"GREY" %c   |"WHT"              ", finish[(1 + (4 * hrac)) % 16], field[(32 + (9 * hrac)) % 36]); vyfarbi(hrac+2);printf("---------"WHT"\n");
    printf("          "); vyfarbi(hrac+3);printf("| %c | %c |"WHT"               "GREY"|_____", house[(2 + (4 * hrac)) % 16], house[(3 + (4 * hrac)) % 16]); vyfarbi(hrac+3);printf("|_____|"GREY"_____|"WHT"              "); vyfarbi(hrac+2);printf("| %c | %c |"WHT"\n", house[(14 + (4 * hrac)) % 16], house[(15 + (4 * hrac)) % 16]);
    printf("          "); vyfarbi(hrac+3);printf("|-------|"WHT"               "GREY"| %c   ", field[(0 + (9 * hrac)) % 36]); vyfarbi(hrac+3);printf("| %c   |"GREY" %c   |"WHT"              ", finish[(0 + (4 * hrac)) % 16], field[(33 + (9 * hrac)) % 36]); vyfarbi(hrac+2);printf("|-------|"WHT"\n");
    printf("          "); vyfarbi(hrac+3);printf("| %c | %c |"WHT"               "GREY"|_____", house[(0 + (4 * hrac)) % 16], house[(1 + (4 * hrac)) % 16]); vyfarbi(hrac+3);printf("|_____|"GREY"_____|"WHT"              "); vyfarbi(hrac+2);printf("| %c | %c |"WHT"\n", house[(12 + (4 * hrac)) % 16], house[(13 + (4 * hrac)) % 16]);
    printf("          "); vyfarbi(hrac+3);printf("---------"WHT"               "GREY"| %c   |  %c  | %c   |"WHT"              ", nastav[(0 + hrac) % 4], field[(35 + (9 * hrac)) % 36], field[(34 + (9 * hrac)) % 36]); vyfarbi(hrac+2);printf("---------"WHT"\n");
    printf("          "WHT      "                        "GREY"|_____|_____|_____|"WHT"\n\n\n");
    printf(BLK DEFAULT_FONT_COLOR);
}

char vyfarbi(int x) {
    switch (x % 4) {
        case 0 :
            printf(GRN);
            break;
        case 1 :
            printf(RED);
            break;
        case 2 :
            printf(BLU);
            break;
        case 3 :
            printf(YEL);
            break;
        default:
            break;
    }
    return 0;
}

void setPlayer(int hrac) {
    house[0 + (hrac * 4)] = 'A';
    house[1 + (hrac * 4)] = 'B';
    house[2 + (hrac * 4)] = 'C';
    house[3 + (hrac * 4)] = 'D';
}

void nastavitfigurku(int hrac, int cislo_figurky, int typ_pola, int index_v_poli) {
    int index = ((hrac * 4) + cislo_figurky); // 15
    char znak = ' ';
    switch (typPola[index]) {
        case (0) :
            znak = house[indexVPoli[index]];
            house[indexVPoli[index]] = ' ';
            break;
        case (1) :
            znak = nastav[indexVPoli[index]];
            nastav[indexVPoli[index]] = 'x';
            break;
        case (2) :
            znak = field[indexVPoli[index]];
            field[indexVPoli[index]] = ' ';
            break;
        case (3) :
            znak = finish[indexVPoli[index]];
            finish[indexVPoli[index]] = ' ';
            break;
        default:
            break;
    }

    typPola[index] = typ_pola;
    if (typ_pola == 3 || typ_pola == 0)
        indexVPoli[index] = index_v_poli + (hrac * 4);
    else if (typ_pola == 1)
        indexVPoli[index] = hrac;
    else
        indexVPoli[index] = index_v_poli;


    switch (typ_pola) {
        case (0) :
            house[index_v_poli + (hrac * 4)] = znak;
            break;
        case (1) :
            nastav[hrac] = znak;
            break;
        case (2) :
            field[index_v_poli] = znak;
            break;
        case (3) :
            finish[index_v_poli + (hrac * 4)] = znak;
            break;
        default:
            break;
    }
}

void vymazHraca(int hrac) {

    for (int i = (hrac * 4); i < (hrac * 4) + 4; i++) {
        nastavitfigurku(hrac, i % 4, 0, 0);
    }

    for (int i = (hrac * 4); i < (hrac * 4) + 4; i++) {
        house[i] = ' ';
    }
}