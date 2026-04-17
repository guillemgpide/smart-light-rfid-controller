/*
 * File:   TAD_Cervell.c
 * Author: danie
 *
 * Created on 26 de febrero de 2025, 15:54
 */


#include <xc.h>
#include "pic18f4321.h"
#include "TAD_CERVELL.h"
#include "TAD_SIO.h"
#include "TAD_PTM.h"
#include "TAD_MTG.h"
#include "TAD_OUT.h"
#include "TAD_TC.h"
#include "TAD_RFID.h"
#include "TAD_LCD.h"

static char fraseUID[24] = {FRASE_UID};
static char ledsChars[6];
static char fraseLeds[48] = {FRASE_LEDS};
static char firstFraseLCD[16] = {FIRST_FRASE_LCD};
static char secondFraseLCD[16] = {SECOND_FRASE_LCD};
static unsigned char UID1[MAX_UID] = {0x45, 0x42, 0xfd, 0x03, 0xf9}; //Tajeta Blanca
static unsigned char UID2[MAX_UID] = {0x93, 0xde, 0x65, 0x2d, 0x05}; //Azul Flojo
static unsigned char UID3[MAX_UID] = {0xef, 0x86, 0x1e, 0xc6, 0xb1}; //Azul Fuerte
static char i, j, z, fine;
static char index, actual;
static char tecla;
static char actualTime[4];
static unsigned char uid[16];
static unsigned int buffer;

static User users[MAX_USERS];

char convertHexToCharacter(char mascara, char c){
    if (mascara < 10){
        c = mascara + '0';
    }else{
        c = mascara - 10 + 'A';
    }
    return c;
}

void CVL_newConfig(void) {
    users[index].leds[PTM_getLed()] = PTM_getIntensitat();
}

void CVL_Init(void) {
    i = j = 0;
    index = (char) - 1;
    
    while(i < MAX_UID){
        users[0].id[i] = UID1[i];
        users[1].id[i] = UID2[i];
        users[2].id[i] = UID3[i];
        i++;
    }
    while(j < MAX_USERS){
        i = 0;
        while(i < MAX_LEDS){
            users[j].leds[i] = 0;
            i++;
        }
        j++;
    }
}

void CVL_Motor(void) {
    static char estat = 61;
    char mascara, c;
    
    switch(estat) {
        case 61:
            if(buffer == 0) {
                RFID_start(uid);
                buffer = 0x400;
            }
            estat = 0;
            break;
        case 60:
            if(buffer == 0) {
                RFID_start(uid);
                buffer = 0x400;
            }
            estat = 1;
            break;
        case 0: //OK
            estat = 15;
            break; 
        case 15: //OK
            i = 0;
            estat = 16;
            break;
        case 16: //OK
            if (FRASE_MENU[i] == '\0') {
                estat = 1;
            } else if (FRASE_MENU[i] != '\0') {
                SIO_PutChar(FRASE_MENU[i]);
                i++;
                estat = 17;
            }
            break;
        case 17: //OK
            if (SIO_TXAvail()) {
                estat = 16;
            }
            break;
        case 1: //OK
            if(RFID_endRead()){
                if(buffer > 0) {
                    buffer--;
                }else{
                    estat = 60;
                }
                if(RFID_statusRead()){
                    i = 0;
                    j = 0;
                    actual = 0;
                    fine = 0;
                    estat = 80;
                }
            }
            if (SIO_RXAvail()) {
                tecla = SIO_GetChar();
                SIO_PutChar(tecla);
                estat = 2;
            }
            if (PTM_getMostraMenuPrincipal()) {
                estat = 0;
            }
            if (PTM_getReset()) {
                OUT_stop();
                estat = 18;
            }
            if (PTM_getConfig() && index != (char) - 1) {
                CVL_newConfig();
                OUT_start();
                OUT_actualizeIntensities(users[index].leds);
                i = 0;
                estat = 40;
            }
            if (TC_actualitzaTime() && index != (char) - 1) {
                TC_modificarTime(firstFraseLCD);
                MTG_showFirstMsg(firstFraseLCD);
                estat = 5;
            }
            break;
        case 80: //OK
            i = 0;
            estat = 81;
            break;
        case 81: //OK
            if (i < MAX_USERS) {
                j = 0;
                fine = 0;
                estat = 82;
            } else {
                estat = 60;
            }
            break;
        case 82: //OK
            if (j < 5) {
                if (users[i].id[j] == uid[j]) {
                    fine++;
                }
                j++;
            } else {
                if (fine != 5) {
                    i++;
                    estat = 81;
                } else {
                    actual = i;
                    estat = 83;
                }
            }
            break;
        case 83: //OK
            if (actual != index) {
                index = actual;
                i = 0;
                estat = 50;
            } else {
                index = (char) - 1;
                LCD_Clear();
                OUT_stop();
                estat = 60;
            }
            break;
        case 84: //OK
            mascara = users[index].id[4] & 0x0F;
            c = convertHexToCharacter(mascara, c);
            firstFraseLCD[0] = c;
            i = 0;
            estat = 40;
            break;
        case 50: //OK
            if (FRASE_RFID[i] == '\0') {
                i = 0;
                estat = 52;
            } else if (FRASE_RFID[i] != '\0') {
                SIO_PutChar(FRASE_RFID[i]);
                i++;
                estat = 51;
            }
            break;
        case 51: //OK
            if (SIO_TXAvail()) {
                estat = 50;
            }
            break;
        case 52: //OK
            if (FRASE_MENU[i] == '\0') {
                estat = 84;
            } else if (FRASE_MENU[i] != '\0') {
                SIO_PutChar(FRASE_MENU[i]);
                i++;
                estat = 53;
            }
            break;
        case 53: //OK
            if (SIO_TXAvail()) {
                estat = 52;
            }
            break;
        case 5: //OK
            if (MTG_getFirstFlag() == 0) {
                estat = 60;
            }
            break;
        case 40: //OK
            if (i < MAX_LEDS) {
                if (users[index].leds[i] < 10) {
                    ledsChars[i] = users[index].leds[i] + '0';
                } else {
                    ledsChars[i] = users[index].leds[i] - 10 + 'A';
                }
                i++;
            } else {
                firstFraseLCD[10] = ledsChars[0];
                firstFraseLCD[14] = ledsChars[1];
                secondFraseLCD[2] = ledsChars[2];
                secondFraseLCD[6] = ledsChars[3];
                secondFraseLCD[10] = ledsChars[4];
                secondFraseLCD[14] = ledsChars[5];
                TC_modificarTime(firstFraseLCD);
                OUT_actualizeIntensities(users[index].leds);
                OUT_start();
                MTG_showFirstMsg(firstFraseLCD);
                estat = 41;
            }
            break;
        case 41: //OK
            if (MTG_getFirstFlag() == 0) {
                LCD_GotoXY(0, 1);
                MTG_showSecondMsg(secondFraseLCD);
                estat = 42;
            }
            break;
        case 42: //OK
            if (MTG_getSecondFlag() == 0) {
                estat = 60;
            }
            break;
        case 18: //OK
            i = j = 0;
            estat = 19;
            break;
        case 19: //OK
            if (j < MAX_USERS) {
                i = 0;
                estat = 20;
            } else if (j == MAX_USERS) {
                if (index != (char) - 1) {
                    i = 0;
                    estat = 40;
                } else {
                    estat = 60;
                }
            }
            break;
        case 20: //OK
            if (i < MAX_LEDS) {
                users[j].leds[i] = 0;
                i++;
            } else if (i == MAX_LEDS) {
                j++;
                estat = 19;
            }
            break;
        case 2: // OK
            if (tecla == '1') {
                i = 0;
                estat = 3;
            } else if (tecla == '2') { 
                i = j = z = 0;
                estat = 4;
            } else if (tecla == '3') {
                estat = 21;
            } else if (tecla == 27) {
                estat = 0;
            } else if (tecla != 27 && tecla != '1' && tecla != '2' && tecla != '3') {
                estat = 26;
            }
            break;
        case 21: //OK
            i = 0;
            estat = 22;
            break;
        case 22: //OK
            if (FRASE_TIME[i] != '\0') {
                SIO_PutChar(FRASE_TIME[i]);
                i++;
                estat = 23;
            } else if (FRASE_TIME[i] == '\0') {
                i = 0;
                estat = 24;
            }
            break;
        case 23: //OK
            if (SIO_TXAvail()) {
                estat = 22;
            }
            break;
        case 24: //OK
            if (SIO_RXAvail()) {
                actualTime[i] = SIO_GetChar();
                SIO_PutChar(actualTime[i]);
                i++;
                if (i == 4) {
                    TC_setNewTime(actualTime);
                    TC_modificarTime(firstFraseLCD);
                    if (index != (char) - 1){
                        MTG_showFirstMsg(firstFraseLCD);
                    }
                    estat = 0;
                }
            }
            break;
        case 3: //OK
            if (index == (char) - 1) {
                estat = 30;
            } else {
                i = 0;
                z = 0;
                estat = 8;
            }
            break;
        case 8: //OK
            if (i < 14) {
                char mascara = (users[index].id[z] & 0xF0) >> 4;
                char c = convertHexToCharacter(mascara, c);
                fraseUID[7 + i] = c;
                i++;
                mascara = users[index].id[z] & 0x0F;
                c = convertHexToCharacter(mascara, c);
                fraseUID[7 + i] = c;
                i++;
                i++;
                z++;
            } else if(i == 15) {
                i = 0;
                estat = 70;
            }
            break;
        case 70:
            if (fraseUID[i] != '\0') {
                SIO_PutChar(fraseUID[i]);
                i++;
                estat = 71;
            } else if (fraseUID[i] == '\0') {
                i = 0;
                estat = 0;
            }
            break;
        case 71: //OK
            if (SIO_TXAvail()) {
                estat = 70;
            }
            break;
        case 26: //OK
            i = 0;
            estat = 27;
            break;
        case 27: //OK
            if (FRASE_ERROR[i] != '\0') {
                SIO_PutChar(FRASE_ERROR[i]);
                i++;
                estat = 28;
            } else if (FRASE_ERROR[i] == '\0') {
                estat = 0;
            }
            break;
        case 28: //OK
            if (SIO_TXAvail()) {
                estat = 27;
            }
            break;
        case 30: //OK
            i = 0;
            estat = 31;
            break;
        case 31: //OK
            if (FRASE_ERROR_USER[i] != '\0') {
                SIO_PutChar(FRASE_ERROR_USER[i]);
                i++;
                estat = 32;
            } else if (FRASE_ERROR_USER[i] == '\0') {
                estat = 0;
            }
            break;
        case 32: //OK
            if (SIO_TXAvail()) {
                estat = 31;
            }
            break;
        case 4: //OK 
            if (j < MAX_USERS) {
                i = z = 0;
                estat = 9;
            } else if (j == MAX_USERS) {
                estat = 0;
            }
            break;
        case 9: //OK
            if (i < 14) {
                char mascara = (users[j].id[z] & 0xF0) >> 4;
                char c = convertHexToCharacter(mascara, c);
                fraseUID[7 + i] = c;
                i++;
                mascara = users[j].id[z] & 0x0F;
                c = convertHexToCharacter(mascara, c);
                fraseUID[7 + i] = c;
                i++;
                i++;
                z++;
            } else if(i == 15) {
                i = 0;
                estat = 10;
            }
            break;
        case 10: //OK
            if (i < MAX_LEDS) {
                if (users[j].leds[i] < 10) {
                    ledsChars[i] = users[j].leds[i] + '0';
                } else {
                    ledsChars[i] = users[j].leds[i] - 10 + 'A';
                }
                i++;
            } else {
                fraseLeds[4] = ledsChars[0];
                fraseLeds[12] = ledsChars[1];
                fraseLeds[20] = ledsChars[2];
                fraseLeds[28] = ledsChars[3];
                fraseLeds[36] = ledsChars[4];
                fraseLeds[44] = ledsChars[5];
                i = 0;
                estat = 11;
            }
            break;
        case 11: //OK
            if (fraseUID[i] != '\0') {
                SIO_PutChar(fraseUID[i]);
                i++;
                estat = 12;
            } else if (fraseUID[i] == '\0') {
                i = 0;
                estat = 13;
            }
            break;
        case 12: //OK
            if (SIO_TXAvail()) {
                estat = 11;
            }
            break;
        case 13: //OK
            if (fraseLeds[i] != '\0') {
                SIO_PutChar(fraseLeds[i]);
                i++;
                estat = 14;
            } else if (fraseLeds[i] == '\0') {
                j++;
                estat = 4;
            }
            break;
        case 14: //OK
            if (SIO_TXAvail()) {
                estat = 13;
            }
            break;
    }
}