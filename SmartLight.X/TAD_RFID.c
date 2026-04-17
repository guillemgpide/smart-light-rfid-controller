/*
 * File:   TRFID.c
 * Author: jnavarro & ester.vidana
 *
 * 
 * Inspired by: https://simplesoftmx.blogspot.com/2014/11/libreria-para-usar-lector-rfid-rc522.html
 */


#include <xc.h>
#include "TAD_RFID.h"

static char start, which, read, isCard, length, _status, bits;
static unsigned int i, bitsInBack;
static unsigned char index;
static unsigned char* id, data, lastData;

//-------------- Private functions: --------------
void InitPortDirections(){
    DIR_MFRC522_SO  = 1; 
    DIR_MFRC522_SI  = 0; 
    DIR_MFRC522_SCK = 0; 
    DIR_MFRC522_CS  = 0; 
    DIR_MFRC522_RST = 0;
}

/*void delay_us(char howMany) {
#define NUM_US 16  //Number of instructions in 1 uS  
    char x;
    for (x=0; x<howMany*NUM_US;x ++) Nop();
}*/

unsigned char MFRC522_Rd(unsigned char Address) {
   unsigned int i, ucAddr;
   unsigned int ucResult = 0;
   MFRC522_SCK = 0;
   MFRC522_CS = 0;
   ucAddr = ( (Address<<1)&0x7E)|0x80;

   for (i = 8; i > 0; i--) {
      MFRC522_SI= ((ucAddr&0x80) == 0x80);
      MFRC522_SCK = 1;
      //delay_us(5);

      ucAddr <<= 1;
      MFRC522_SCK = 0;
      //delay_us(5);
   }

   for (i = 8; i > 0; i--) {
      MFRC522_SCK = 1;
      //delay_us(5);
      ucResult <<= 1;   
      ucResult|= MFRC522_SO;
      MFRC522_SCK = 0;
      //delay_us(5);
   }
   MFRC522_CS = 1;
   MFRC522_SCK = 1;
   return ucResult;
}

void MFRC522_Wr(unsigned char Address, unsigned char value){
   unsigned char i, ucAddr;
   MFRC522_SCK = 0;
   MFRC522_CS = 0;
   ucAddr = ( (Address<<1)&0x7E);
   for (i = 8; i > 0; i--) {
      MFRC522_SI= ( (ucAddr&0x80) == 0x80);
      MFRC522_SCK = 1;
      ucAddr <<= 1;
      //delay_us(5);
      MFRC522_SCK = 0;
      //delay_us(5);
   }

   for (i = 8; i > 0; i--) {
      MFRC522_SI = ( (value&0x80) == 0x80);
      MFRC522_SCK = 1;
      value <<= 1;
      //delay_us(5);
      MFRC522_SCK = 0;
      //delay_us(5);
   }
   MFRC522_CS = 1;
   MFRC522_SCK = 1;
}

void MFRC522_Clear_Bit( char addr, char mask ) {     
    unsigned char  tmp =0x0;
    tmp=MFRC522_Rd( addr ) ;
    MFRC522_Wr( addr,  tmp&~mask );   
}

void MFRC522_Set_Bit( char addr, char mask ) {    
    unsigned char  tmp =0x0;
    tmp=MFRC522_Rd( addr ) ; 
    MFRC522_Wr( addr, tmp|mask );
}

void MFRC522_Reset() { 
   MFRC522_RST = 1;
   //delay_us (1);
   MFRC522_RST = 0;
   //delay_us (1);
   MFRC522_RST = 1;
   //delay_us (1);
   MFRC522_Wr( COMMANDREG, PCD_RESETPHASE );
   //delay_us (1);
}

void MFRC522_AntennaOn() {                                              
    unsigned char stt;
    stt= MFRC522_Rd( TXCONTROLREG ) ;
    MFRC522_Set_Bit( TXCONTROLREG, 0x03 );
}

void MFRC522_AntennaOff() {
    MFRC522_Clear_Bit( TXCONTROLREG, 0x03 );                                          
}

void MFRC522_Init() {                 
    MFRC522_CS = 1; 
    MFRC522_RST = 1;
   
    MFRC522_Reset();       
    MFRC522_Wr( TMODEREG, 0x8D );      //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    MFRC522_Wr( TPRESCALERREG, 0x3E ); //TModeReg[3..0] + TPrescalerReg
    MFRC522_Wr( TRELOADREGL, 30 );
    MFRC522_Wr( TRELOADREGH, 0 ); 
    MFRC522_Wr( TXAUTOREG, 0x40 );    //100%ASK
    MFRC522_Wr( MODEREG, 0x3D );      // CRC valor inicial de 0x6363


    MFRC522_AntennaOff() ;            
    MFRC522_AntennaOn();
}

char MFRC522_ToCard( char command, char *sendData, char sendLen, char *backData, unsigned *backLen ) {
    char _status = MI_ERR;
    char irqEn = 0x00;
    char waitIRq = 0x00;               
    char lastBits;
    char n;
    unsigned i;

    switch (command) {
        case PCD_AUTHENT:       //Certification cards close
            irqEn = 0x12;
            waitIRq = 0x10;
            break;
        case PCD_TRANSCEIVE:    //Transmit FIFO data
            irqEn = 0x77;
            waitIRq = 0x30;
            break;
        default:
            break;
    }
    
    MFRC522_Wr( COMMIENREG, irqEn | 0x80 );  //Interrupt request
    MFRC522_Clear_Bit( COMMIRQREG, 0x80 );   //Clear all interrupt request bit
    MFRC522_Set_Bit( FIFOLEVELREG, 0x80 );   //FlushBuffer=1, FIFO Initialization
    MFRC522_Wr( COMMANDREG, PCD_IDLE );      //NO action; Cancel the current command???

    for ( i=0; i < sendLen; i++ ) MFRC522_Wr( FIFODATAREG, sendData[i] );

    MFRC522_Wr( COMMANDREG, command );
    if (command == PCD_TRANSCEIVE ) MFRC522_Set_Bit( BITFRAMINGREG, 0x80 ); //StartSend=1,transmission of data starts 
    i = 0xFFFF;  //Drey
    do {
        n = MFRC522_Rd( COMMIRQREG );
        i--;
    } while ( i && !(n & 0x01) && !( n & waitIRq ) );
    MFRC522_Clear_Bit( BITFRAMINGREG, 0x80 );   
    if (i != 0) {
        if( !( MFRC522_Rd( ERRORREG ) & 0x1B ) ) {
            _status = MI_OK;
            if ( n & irqEn & 0x01 ) _status = MI_NOTAGERR;      
            if ( command == PCD_TRANSCEIVE ) {
                n = MFRC522_Rd( FIFOLEVELREG );
                lastBits = MFRC522_Rd( CONTROLREG ) & 0x07;
                if (lastBits) *backLen = (n-1) * 8 + lastBits;
                else *backLen = n * 8;
                if (n == 0) n = 1;
                else if (n > 16) n = 16;

                for (i=0; i < n; i++) backData[i] = MFRC522_Rd( FIFODATAREG );
                backData[i] = 0;
            }
        }
      else _status = MI_ERR;
    }
    return _status;
}

char MFRC522_Request( char reqMode, char *TagType ) {
    char _status;
    unsigned backBits;           
    MFRC522_Wr( BITFRAMINGREG, 0x07 );
    TagType[0] = reqMode;
    _status = MFRC522_ToCard( PCD_TRANSCEIVE, TagType, 1, TagType, &backBits );
    if ( (_status != MI_OK) || (backBits != 0x10) ) {
        _status = MI_ERR;
    }
    return _status;
}

void MFRC522_CRC( char *dataIn, char length, char *dataOut ) {
    char i, n;
    MFRC522_Clear_Bit( DIVIRQREG, 0x04 );
    MFRC522_Set_Bit( FIFOLEVELREG, 0x80 );
    
    for ( i = 0; i < length; i++ ) MFRC522_Wr( FIFODATAREG, *dataIn++ );  
       
    MFRC522_Wr( COMMANDREG, PCD_CALCCRC );
       
    i = 0xFF; //no tocar
 
    do{
        n = MFRC522_Rd( DIVIRQREG );
        i--;
    } while( i && !(n & 0x04) );        //CRCIrq = 1
       
    dataOut[0] = MFRC522_Rd( CRCRESULTREGL );
    dataOut[1] = MFRC522_Rd( CRCRESULTREGM );       
}

char MFRC522_SelectTag( char *serNum ) {
    char i;
    char _status;
    char size;
    unsigned recvBits;
    char buffer[9];


    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;

    for ( i=2; i < 7; i++ ) buffer[(int)i] = *serNum++;

    MFRC522_CRC( buffer, 7, &buffer[7] );            

    _status = MFRC522_ToCard( PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits );
    if ( (_status == MI_OK) && (recvBits == 0x18) ) size = buffer[0];
    else size = 0;
    
    return size;
}

//hibernation
void MFRC522_Halt() {
    unsigned unLen;
    char buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    MFRC522_CRC( buff, 2, &buff[2] );
    MFRC522_Clear_Bit( STATUS2REG, 0x80 );
    MFRC522_ToCard( PCD_TRANSCEIVE, buff, 4, buff, &unLen );
    MFRC522_Clear_Bit( STATUS2REG, 0x08 );
}

char MFRC522_AntiColl( char *serNum ) {
    char _status;
    char i;
    char serNumCheck = 0;
    unsigned unLen;
    MFRC522_Wr( BITFRAMINGREG, 0x00 );                //TxLastBists = BitFramingReg[2..0]
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    MFRC522_Clear_Bit( STATUS2REG, 0x08 );
    _status = MFRC522_ToCard( PCD_TRANSCEIVE, serNum, 2, serNum, &unLen );
    if (_status == MI_OK) {
        for ( i=0; i < 4; i++ ) serNumCheck ^= serNum[(int)i];  
        if ( serNumCheck != serNum[4] ) _status = MI_ERR;
    }
    return _status;
}

char MFRC522_isCard( char *TagType ) {
    if (MFRC522_Request( PICC_REQIDL, TagType ) == MI_OK)
        return 1;
    else
        return 0;
}

char MFRC522_ReadCardSerial( unsigned char *str ) {
    char _status;
    _status = MFRC522_AntiColl( (char *) str );
    str[5] = 0;
    if (_status == MI_OK)
        return 1;
    else
        return 0;
}

//-------------- Public functions: --------------
void RFID_Init(void){
    InitPortDirections();
    MFRC522_Init(); 
}


void ReadRFID_NoCooperatiu(){
    unsigned char UID[16];   
    char TagType;                       

    if (MFRC522_isCard (&TagType))       {      
       //At this point, TagType contains an integer value corresponding to the type of card.
       //Read ID
       if (MFRC522_ReadCardSerial (UID)) {
           i = 0;
           //At this point, UID contains the value of the card.
       }                                      
       MFRC522_Halt () ;
    }   
}

void RFID_start(unsigned char* uid) {
    start = 1;
    id = uid;
}

char RFID_endRead(void) {
    return !start;
}

char RFID_statusRead(void) {
    char aux = read;
    read = 0;
    return aux;
}

void RFID_Motor(void) {
    static char estat = 0;

    switch (estat) {
        case 0:
            if (start) {
                estat = 1;
            }
            break;

        case 1:
            MFRC522_Wr(BITFRAMINGREG, 0x07);
            id[0] = PICC_REQALL;
            length = 1;
            _status = MI_ERR;
            isCard = 1;
            estat = 3;
            break;

        case 2:
            MFRC522_Wr(BITFRAMINGREG, 0x00);
            id[0] = PICC_ANTICOLL;
            id[1] = 0x20;
            MFRC522_Clear_Bit(STATUS2REG, 0x08);
            length = 2;
            data = lastData = id;
            _status = MI_ERR;
            isCard = 0;
            which = 0;
            estat = 3;
            break;

        case 3:
            MFRC522_Wr(COMMIENREG, 0xF7);
            MFRC522_Clear_Bit(COMMIRQREG, 0x80);
            estat = 4;
            break;

        case 4:
            MFRC522_Set_Bit(FIFOLEVELREG, 0x80);
            MFRC522_Wr(COMMANDREG, PCD_IDLE);
            estat = 5;
            break;

        case 5:
            MFRC522_Wr(FIFODATAREG, id[0]);
            if (!isCard) {
                MFRC522_Wr(FIFODATAREG, id[1]);
            }
            estat = 6;
            break;

        case 6:
            MFRC522_Wr(COMMANDREG, PCD_TRANSCEIVE);
            MFRC522_Set_Bit(BITFRAMINGREG, 0x80);
            i = 0xFFFF;
            index = 0;
            estat = 8;
            break;

        case 8:
            if (i > 0) {
                index = MFRC522_Rd(COMMIRQREG);
                if ((index & 0x01) || (index & 0x30)) {
                    MFRC522_Clear_Bit(BITFRAMINGREG, 0x80);
                    which = 0;
                    estat = 9;
                } else {
                    i--;
                }
            } else {
                MFRC522_Clear_Bit(BITFRAMINGREG, 0x80);
                which = 0;
                estat = 9;
            }
            break;

        case 9:
            if (i > 0) {
                estat = 10;
            } else {
                read = 0;
                start = 0;
                estat = 0;
            }
            break;

        case 10:
            if (!(MFRC522_Rd(ERRORREG) & 0x1B)) {
                _status = MI_OK;
                if ((index & 0x77) & 0x01) {
                    _status = MI_NOTAGERR;
                }
                index = MFRC522_Rd(FIFOLEVELREG);
                bits = MFRC522_Rd(CONTROLREG) & 0x07;
                bitsInBack = (index ? (index - 1) * 8 + bits : index * 8);
                index = (index == 0) ? 1 : (index > 16 ? 16 : index);
                i = 0;
                estat = 12;
            } else {
                _status = MI_ERR;
                start = 0;
                estat = 0;
            }
            break;

        case 11:
            estat = (isCard == 1) ? 13 : 14;
            break;

        case 12:
            if (i < index) {
                id[i++] = MFRC522_Rd(FIFODATAREG);
            } else {
                id[i] = 0;
                estat = 11;
            }
            break;

        case 13:
            if ((_status != MI_OK) || (bitsInBack != 0x10)) {
                _status = MI_ERR;
                read = 0;
                start = 0;
                estat = 0;
            } else {
                isCard = 0;
                estat = 2;
            }
            break;

        case 14:
            estat = (_status == MI_OK) ? 16 : 15;
            if (_status == MI_OK) {
                i = 0;
            }
            break;

        case 15:
            id[5] = 0;
            start = 0;
            read = (_status == MI_OK) ? 1 : 0;
            estat = 0;
            break;

        case 16:
            if (i < 4) {
                which ^= id[(int)i++];
            } else {
                if (which != id[4]) {
                    _status = MI_ERR;
                }
                estat = 15;
            }
            break;
    }
}
