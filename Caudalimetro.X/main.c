/*
 * File:   main.c
 * Author: EAVELLANEDA
 *
 * Created on 28 de septiembre de 2021, 15:36
 */


#include "config.h"
#include "lcd.h"
#include "Temporizadores.h"
#include "UART.h"
#include "stdio.h"

//SALIDAS DE LED
#define LED_ERROR LATDbits.LATD1
#define LED_DEBUG LATDbits.LATD0

//CONTROL BUFFER
#define TF_TX_to_PIC_RX     LATAbits.LA0
#define PIC_TX_to_TF_RX     LATAbits.LA1
#define PIC_TX_to_SIM_RX    LATAbits.LA2
#define SIM_TX_to_PIC_RX    LATAbits.LA3

//CONTROL DE SIM
#define SIM_RESET   LATCbits.LATC0
#define SIM_ENABLE  LATCbits.LATC1
unsigned char Buffer[30];
unsigned char puntero = 0;

//ENTRADA DE SEÑAL
#define SIGNAL  PORTCbits.RC2

//FUNCIONES DE CONTROL
#define GSM  0
#define TF02 1
void Select_Mode(char modo);
int flag_tempo0 = 0;
int flag_tempo1 = 0;
int flag_serial = 0;
int contador_t1 = 0;
int llamada_ok = 0;

void __interrupt() scr(){
    if(PIR1bits.RC1IF){
        
        char c = RCREG;
        if(c != 0){
            LED_DEBUG = 1;
            LED_ERROR++;
            if(puntero < 30){
                Buffer[puntero] = c;
                puntero++;
                TMR0 = 0;
                T0CONbits.TMR0ON = 1;
            }
        }
        PIR1bits.RCIF = 0;
    }
    if(INTCONbits.TMR0IF){
        flag_tempo0 = 1;
        T0CONbits.TMR0ON = 0;
        INTCONbits.TMR0IF = 0;
    }
    if(PIR1bits.TMR1IF){
        contador_t1++;
        if(contador_t1>=100){
            LED_ERROR ++;
            flag_tempo1 = 1;
            contador_t1 = 0;
        }
        PIR1bits.TMR1IF = 0;
    }
}


void main(void){
    /*CONFIGURACION DE PINES*/
    ADCON1bits.PCFG = 0xF; // TODO COMO DIGITAL
    TRISA = 0xF0;
    TRISC = 0b11111100;
    TRISDbits.RD0 = 0;
    TRISDbits.RD1 = 0;
    LATA = 0;
    LATC = 0;
    LATD = 0;
    
    /*INTERRUPCIONES*/
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    /*TEMPORIZADORES*/
    Timer0_Init(100);
    Timer1_Init(100);
    T0CONbits.TMR0ON = 0;
    
    /*INICIO DE LCD*/
    Lcd_Init();
    Lcd_Cmd(LCD_CURSOR_OFF);
    Lcd_Out(1,0,"CAUDALIMETRO");
    
    /*COMUNICACION UART*/
    UART_Init();
    UART_Begin(9600);
    
    /*INICIO SIM800*/
    SIM_ENABLE = 1;
    SIM_RESET = 0;
    Select_Mode(TF02);
    __delay_ms(3000);
    __delay_ms(3000);
    __delay_ms(3000);
    while(1){
        if(flag_tempo1){
            if(!llamada_ok){
                UART_Println("ATD929105967;");
                llamada_ok = 1;
            }
            else{
                UART_Println("AT");
            }
            flag_tempo1 = 0;
        }
        
        if(flag_tempo0){
            for(unsigned char i = 0; i<puntero; i++){
                Lcd_getc(2,i,Buffer[i]);
            }
            puntero = 0;
            flag_tempo0 = 0;
            LED_DEBUG = 0;
        }
    }
}

void Select_Mode(char modo){
    switch(modo){
        case GSM: 
            PIC_TX_to_TF_RX = 1;
            TF_TX_to_PIC_RX = 1;
            PIC_TX_to_SIM_RX = 0;
            SIM_TX_to_PIC_RX = 0;
            break;
        case TF02: 
            PIC_TX_to_SIM_RX = 1;
            SIM_TX_to_PIC_RX = 1;
            PIC_TX_to_TF_RX = 0;
            TF_TX_to_PIC_RX = 0;
            break;
    }
}