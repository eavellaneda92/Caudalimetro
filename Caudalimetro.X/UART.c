#include "UART.h"

void UART_Init(void){
    /*TXSTA*/
    TXSTAbits.SYNC = 0; //Asincrono
    TXSTAbits.TXEN = 1; //Transmision en ON
    TXSTAbits.TX9 = 0;  //8 bits
    /*RCSTA*/
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    //BAUD
    TXSTAbits.BRGH = 1;
    BAUDCONbits.BRG16 = 0;
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 1;
}

void UART_Begin(unsigned long baud){
    double aux = _XTAL_FREQ;
    double aux2 = baud;
    aux = aux / aux2;
    if(baud <= 19200){
        TXSTAbits.BRGH = 0;
        aux = aux / 64.0;
    }else{
        TXSTAbits.BRGH = 1;
        aux = aux / 16.0;
    }
    aux = aux - 1.0;
    baud = (unsigned long)aux;
    if(aux - baud >= 0.5){
        baud++;
    }
    SPBRG = (unsigned char)baud;
}

void UART_Write( char data){
    TXREG=data;
    while(!TXSTAbits.TRMT);    
}

void UART_Println(char *buffer){
    UART_Print(buffer);
    TXREG=10;
    while(!TXSTAbits.TRMT);
}

void UART_Print(char *buffer){
    while(*buffer){
        UART_Write(*buffer);
        buffer++;
    }
}
