#ifndef TAD_TM_H
#define	TAD_TM_H

#define MAX_FILA 4
#define Col0 PORTCbits.RC4
#define Col1 PORTCbits.RC5
#define Col2 PORTDbits.RD0

void TM_Init(void);
char TM_hiHaTecla(void);
char TM_noHiHaTecla(void);
char TM_getTecla(void);

void TM_Motor(void);

#endif