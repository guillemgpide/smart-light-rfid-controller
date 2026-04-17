/*
 * File:   TAD_TM.c
 * Author: danie
 *
 * Created on 26 de febrero de 2025, 13:18
 */


#include <xc.h>
#include "TAD_TM.h"
#include "TAD_TIMER.h"
#include "pic18f4321.h"

static unsigned char timer;
static char hiHaTecla, noHiHaTecla, tecla, fila, flag_aux;
static char C0[MAX_FILA] = {'1', '4', '7', '*'};
static char C1[MAX_FILA] = {'2', '5', '8', '0'};
static char C2[MAX_FILA] = {'3', '6', '9', '#'};

void TM_Init(void){
    TI_NewTimer(&timer);
    TRISCbits.TRISC0 = TRISCbits.TRISC1 = TRISCbits.TRISC2 = TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = TRISCbits.TRISC5 = TRISDbits.TRISD0 = 1;
    LATCbits.LATC0 = 0;
    LATCbits.LATC1 = LATCbits.LATC2 = LATCbits.LATC3 = 1;
    INTCON2bits.RBPU = 0;
    
    fila = 0xFE;
    LATC = (LATC & 0xF0) | (fila & 0x0F);
    hiHaTecla = 0;
    flag_aux = 0;
}

char TM_hiHaTecla(void) {
    char aux;
    
    aux = hiHaTecla;
    hiHaTecla = 0;
    
    return aux;
}

char TM_noHiHaTecla(void) {
    char aux;
    
    aux = noHiHaTecla;
    noHiHaTecla = 0;
    
    return aux;
}

char TM_getTecla(void){
    return tecla;
}

char TM_premut(void){
    return !(Col0 & Col1 & Col2);
}

void TM_rotateF(void){
    fila = (char)(fila << 1) | 0x01;
    flag_aux++;
    if(flag_aux == 4){
        fila = 0xFE;
        flag_aux = 0;
    }
    LATC = (LATC & 0xF0) | (fila & 0x0F);
}

void saveTecla(void){
    if(Col0 == 0) {
        tecla = C0[flag_aux];
    }
    if(Col1 == 0){
        tecla = C1[flag_aux];
    }
    if (Col2 == 0){
        tecla = C2[flag_aux];
    }
}

void TM_Motor(void) {
    static char estat = 0;
    
    switch(estat) {
        case 0:
            TM_rotateF();
            if(TM_premut()) {
                TI_ResetTics(timer);
                estat = 1;
            }
            
            break;
        case 1:
            if(TI_GetTics(timer) >= 50){
                if (TM_premut()) {
                    noHiHaTecla = 0;
                    saveTecla();
                    hiHaTecla = 1;
                    estat = 2;
                } else {
                    estat = 0;
                }
            }
            
            break;
        case 2:
            if(!TM_premut()) {
                noHiHaTecla = 1;
                estat = 0;
            }
            
            break;
    }
}