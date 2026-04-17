/*
 * File:   TAD_SIO.c
 * Author: danie
 *
 * Created on 26 de febrero de 2025, 10:24
 */

#include <xc.h>
#include "pic18f4321.h"
#include "TAD_SIO.h"

void SIO_Init(void) {
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;
    
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;    
    RCSTAbits.SPEN = 1;    
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
    BAUDCONbits.BRG16 = 1;
    SPBRGH = 0;         
    SPBRG = 207;
}

char SIO_RXAvail(void) {
    return (PIR1bits.RCIF == 1) ? 1 : 0;
}

char SIO_GetChar(void) {
    return RCREG;
}

char SIO_TXAvail(void) {
    return TXSTAbits.TRMT;
}

void SIO_PutChar(char Valor) {
    while(TXSTAbits.TRMT == 0);
    TXREG = Valor;
    TXSTAbits.TXEN = 1;
}