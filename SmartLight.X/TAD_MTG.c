/*
 * File:   TAD_MTG.c
 * Author: danie
 *
 * Created on 1 de marzo de 2025, 18:03
 */


#include <xc.h>
#include "pic18f4321.h"
#include "TAD_MTG.h"
#include "TAD_LCD.h"
#include "TAD_TIMER.h"

static char *frase, firstFrase, secondFrase, i;

void MTG_Init(void){
    firstFrase = 0;
    secondFrase = 0;
}

void MTG_showFirstMsg(char *msg) {
    firstFrase = 1;
    frase = msg;
}

void MTG_showSecondMsg(char *msg) {
    secondFrase = 1;
    frase = msg;
}

char MTG_getFirstFlag(void) {
    return firstFrase;
}

char MTG_getSecondFlag(void) {
    return secondFrase;
}

void MTG_Motor(void){
    static char estat = 0;
    
    switch(estat) {
        case 0:
            if(firstFrase == 1 || secondFrase == 1) {
                LCD_GotoXY(0, secondFrase);
                i = 0;
                estat = 1;
            }
            break;
        case 1:
            if(frase[i] != '\0') {
                LCD_PutChar(frase[i]);
                i++;
            } else {
                firstFrase = 0;
                secondFrase = 0;
                estat = 0;
            }
            break;
    }
}