/*
 * File:   TAD_OUT.c
 * Author: danie
 *
 * Created on 24 de marzo de 2025, 13:42
 */


#include <xc.h>
#include "pic18f4321.h"
#include "TAD_TIMER.h"
#include "TAD_OUT.h"

static unsigned char *intensities;
static unsigned char timer, i, start, pwm = 0;
static char first_mask[6] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
static char second_mask[6] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF};

void OUT_Init(void) {
    TRISA = 0x00;
    LATA = 0x00;
    TI_NewTimer(&timer);
    
}

void OUT_actualizeIntensities(char* new) {
    intensities = new;
}

void OUT_start(void){
    start = 1;
}

void OUT_stop(void){
    start = 0;
    LATA = 0x00;
}

void OUT_check(void){
    pwm++;
    if (pwm >= 10){
        pwm = 0;
    }
    i = 0;
}

void OUT_makeIntensities(void){
    if (intensities[i] == 10){
        LATA |= first_mask[i];
    } else {
        if (pwm < intensities[i]){
            LATA |= first_mask[i];
        } else {
            LATA &= second_mask[i];
        }
    }
    i++;
}

void OUT_Motor(void) {
    static char estat = 0;
    
    switch (estat) {
        case 0:
            if (start) {
                TI_ResetTics(timer);
                i = 0;
                estat = 1;
            }
            break;
        case 1:
            if (TI_GetTics(timer) >= 2) {
                OUT_check();
                estat = 2;        
            }
            break;
        case 2:
            if (i < 6) {
                OUT_makeIntensities();
            } else {
                estat = 0;
            }
            break;
    }
}