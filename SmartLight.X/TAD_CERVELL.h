#ifndef TAD_CERVELL_H
#define	TAD_CERVELL_H

#define FRASE_UID "\n\rUID: XX-XX-XX-XX-XX\n\r\0"
#define FRASE_LEDS "L0: X - L1: X - L2: X - L3: X - L4: X - L5: X\n\r\0"
#define FRASE_TIME "\n\rTime: \0"
#define FRASE_ERROR "\n\rThis option is not valid.\n\r\0"
#define FRASE_ERROR_USER "\n\rNo user in the room now.\0"
#define FRASE_MENU "\n\n\r---------------\n\rMenu principal\n\r---------------\n\rTria una opcio:\n\r\t1. Qui hi ha a la sala?\n\r\t2. Mostrar configuracions\n\r\t3. Modificar hora del sistema\n\rOpcio: \0"
#define FIRST_FRASE_LCD "X XX:XX 1-X 2-X\0"
#define SECOND_FRASE_LCD "3-X 4-X 5-X 6-X\0"
#define FRASE_RFID "\n\rTargeta Detectada!\r\0"
#define MAX_UID 5
#define MAX_LEDS 6
#define MAX_USERS 3

typedef struct {
    char id[MAX_UID];
    char leds[MAX_LEDS];
} User;

void CVL_Init(void);
void CVL_Motor(void);

#endif