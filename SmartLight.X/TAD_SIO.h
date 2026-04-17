#ifndef TAD_SIO_H
#define	TAD_SIO_H

#define FALS 0
#define CERT 1

void SIO_Init(void);
//Pre: -
//Post: Configuració de la IART en mode assincron, 9600 bauds, assumint Fosc=10Mhz

char SIO_RXAvail(void);
//Pre: -
//Post: Retorna CERT si hi ha un caràcter rebut pendent de ser llegit, FALS en cas contrari.

char SIO_GetChar(void);
//Pre: SIO_CharAvail() ha retornat CERT.
//Post: Fa lectura destructiva del caràcter rebut.

char SIO_TXAvail(void);
//Pre: -
//Post: Retorna CERT si hi ha disponibilitat per a enviar un caràcter, FALS en cas contrari.

void SIO_PutChar(char Valor);
//Pre: SIO_TXAvail() ha retornat CERT.
//Post: Fa lectura destructiva del caràcter rebut.

void SIO_Close(void);
//Post: Destructor del TAD. No fa res, realment ...

#endif

