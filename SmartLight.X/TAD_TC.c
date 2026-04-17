/*
 * File:   TAD_TC.c
 * Author: danie
 *
 * Created on 5 de marzo de 2025, 18:40
 */


#include <xc.h>
#include "pic18f4321.h"
#include "TAD_TIMER.h"
#include "TAD_TC.h"

static char actual[4], flagNewTime;
static unsigned char timer;

void TC_Init(void){
    TI_NewTimer(&timer);
    actual[0] = '0';
    actual[1] = '0';
    actual[2] = '0';
    actual[3] = '0';
    flagNewTime = 0;
}

char TC_actualitzaTime(void){
    char aux = flagNewTime;
    flagNewTime = 0;
    return aux;
}

void TC_setNewTime(char* actualtime) {
    actual[0] = actualtime[0];
    actual[1] = actualtime[1];
    actual[2] = actualtime[2];
    actual[3] = actualtime[3];
}

void TC_modificarTime(char* frase) {
    frase[2] = actual[0];
    frase[3] = actual[1];
    frase[5] = actual[2];
    frase[6] = actual[3];
}

void TC_moveTime(void) {
    flagNewTime = 1;

    if (actual[3] < '9') {
        actual[3]++;
        return;
    }
    actual[3] = '0';

    if (actual[2] < '5') {
        actual[2]++;
        return;
    }
    actual[2] = '0';

    if (actual[1] < '9') {
        actual[1]++;
    } else {
        actual[1] = '0';

        if (actual[0] == '2' && actual[1] == '3') {
            actual[0] = '0';
            actual[1] = '0';
        } else {
            actual[0]++;
        }
    }

    if (actual[0] == '2' && actual[1] == '4' && actual[2] == '0' && actual[3] == '0') {
        actual[0] = '0';
        actual[1] = '0';
        actual[2] = '0';
        actual[3] = '0';
    }
}


void TC_Motor(void){
    static char estat = 0;
    
    switch(estat) {
        case 0:
            TI_ResetTics(timer);
            estat = 1;
            break;
        case 1:
            if (TI_GetTics(timer) > 60000) {
                estat = 2;
            }
            break;
        case 2:
            TC_moveTime();
            estat = 0;
            break;
    }
}
