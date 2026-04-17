/*
 * File:   TAD_PTM.c
 * Author: danie
 *
 * Created on 26 de febrero de 2025, 17:45
 */


#include <xc.h>
#include "pic18f4321.h"
#include "TAD_PTM.h"
#include "TAD_TM.h"
#include "TAD_TIMER.h"

static char tecla, config, menu, reset, intensitat, led, start;
static unsigned char timer;

char PTM_getStart(void){
    return start;
}

char PTM_getLed(void){
    return led;
}

char PTM_getConfig(void){
    char aux = config;
    config = 0;
    return aux;
}

char PTM_getIntensitat(void) {
    return intensitat;
}

char PTM_getReset(void){
    char aux = reset;
    reset = 0;
    return aux;
}

char PTM_getMostraMenuPrincipal(void){
    char aux = menu;
    menu = 0;
    return aux;
}

void PTM_Init(void) {
    TI_NewTimer(&timer);
}

void PTM_Motor(void) {    
    static char estat = 0;

    switch (estat) {
        case 0:
            if (TM_hiHaTecla()) {
                tecla = TM_getTecla();
                estat = 1;
            }
            break;
        case 1:
            if (tecla == '#') {
                TI_ResetTics(timer);
                estat = 4;
            } else if (tecla >= '0' && tecla < '6') {
                led = tecla - '0';
                estat = 2;
            } else {
                estat = 0;
            }
            break;
        case 2:
            if (TM_hiHaTecla()) {
                tecla = TM_getTecla();
                estat = 3;
            }
            break;
        case 3:
            intensitat = (tecla >= '0' && tecla <= '9' && tecla != '*') ? (tecla - '0') : 10;
            config = 1;
            estat = 0;
            break;
        case 4:
            if (TM_noHiHaTecla()) {
                estat = 0;
                if (TI_GetTics(timer) > 3000) {
                    reset = 1;
                } else {
                    menu = 1;
                }
            }
            break;
    }
}
