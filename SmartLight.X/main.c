/*
 * File:   main.c
 * Author: danie
 *
 * Created on 26 de febrero de 2025, 10:23
 */


#include <xc.h>
#include "pic18f4321.h"
#include "TAD_SIO.h"
#include "TAD_TIMER.h"
#include "TAD_CERVELL.h"
#include "TAD_TM.h"
#include "TAD_PTM.h"
#include "TAD_MTG.h"
#include "TAD_LCD.h"
#include "TAD_TC.h"
#include "TAD_OUT.h"
#include "TAD_RFID.h"

#pragma config OSC = INTIO2
#pragma config PBADEN = DIG
#pragma config WDT = OFF
#pragma config MCLRE = OFF
#pragma config LVP = OFF

void OSC_Init(void) {
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
}

void __interrupt(high_priority) LaRSI (void) {
    if(TMR0IF == 1) {
        RSI_Timer0();
    }
}

void main(void) {
    OSC_Init();
    SIO_Init();
    TI_Init();
    CVL_Init();
    TM_Init();
    PTM_Init();
    LCD_Init(2, 16);
    LCD_CursorOff();
    MTG_Init();
    TC_Init();
    OUT_Init();
    RFID_Init();
        
    while(1) {
        TM_Motor();
        CVL_Motor();
        OUT_Motor();
        PTM_Motor();
        MTG_Motor();
        TC_Motor();
        RFID_Motor();
    }
}
