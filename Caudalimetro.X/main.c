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
#include "sim800.h"
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
char tipo_modo = 0;
char flag_inicio = 0;
char flag_correcto = 0;
unsigned int Valor_h = 0;
unsigned int Valor_l = 0;

//FUNCIONES PARA TRAMAS
int Get_Index(char *TRAMA, char *PARTE);
char Get_Length(char *TRAMA);

//Variables de tiempo
char flag_Buffer = 0;   //Cuando ya registro valores en buffer
char flag_tempo1 = 0;   //Cuando se activa el timer1 
int contador_t1 = 0;    //Acumulador de interrupcion de 25ms 
char flag_tempo2 = 0;   //Cuando termina proceso x de envio GPRS
int contador_t2 = 0;    //Acumulador de interrupcion de 5ms
char flag_send = 0;     //Cuando debe iniciar exportacion de datos

void __interrupt() scr(){
    //INTERRUPCION SERIAL
    if(PIR1bits.RC1IF){
        char c = RCREG;
        if(c != 0){
            if(tipo_modo == TF02){
                if(flag_correcto == 1){
                    if(puntero < 30){
                        Buffer[puntero] = c;
                        puntero++;
                        TMR0 = 0;
                        T0CONbits.TMR0ON = 1;
                    }
                }
                //Pregunta por el segundo 0x59
                if(c == 0x59 && flag_inicio == 1){
                    flag_correcto = 1;
                }else{
                    flag_correcto = 0;
                    flag_inicio = 0;
                }
                //Pregunta por el primer 0x59
                if(c == 0x59){
                    flag_inicio = 1;
                }
            }else{
                
            }
        }
        PIR1bits.RCIF = 0;
    }
    
    //INTERRUPCION TIMER 0
    if(INTCONbits.TMR0IF){
        flag_Buffer = 1;
        T0CONbits.TMR0ON = 0;
        INTCONbits.TMR0IF = 0;
    }
    
    //INTERRUPCION TIMER 1 25ms
    if(PIR1bits.TMR1IF){
        contador_t1++;
        if(contador_t1>=240){ //6 segundo
            LED_ERROR ++;
            flag_tempo1 = 1;
            contador_t1 = 0;
        }
        TMR1 = 28036;
        PIR1bits.TMR1IF = 0;
    }
    
    //INTERRUPCION TIMER 2 5ms
    if(PIR1bits.TMR2IF){
        contador_t2++;
        if(contador_t2>=500){
            
        }
        PIR1bits.TMR2IF = 0;
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
    Timer0_Init();
    Timer1_Init();
    Timer2_Init();
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
    UART_Begin(9600);
    Select_Mode(TF02);

    while(1){
        //LECTURA DE VALORES GUARDADOS EN EL BUFFER
        if(flag_Buffer){
            if(tipo_modo == TF02){
                unsigned int dh = (unsigned int)Buffer[0];
                unsigned int dl = (unsigned int)Buffer[1];
                unsigned int dis = (dh<<8) | dl;
                Select_Mode(GSM);
            }
            flag_Buffer = 0;
        }
        
        //Temporizador para peticion de dato cada 6 segundos
        if(flag_tempo1){
            Select_Mode(TF02);
            flag_tempo1 = 0;
        }
    }
}

int Get_Index(char *TRAMA, char *PARTE){
    
    return 0;
}

char Get_Length(char *TRAMA){
    char contador = 0;
    while(*TRAMA){
        contador++;
        TRAMA++;
    }
    return contador;
}

void Select_Mode(char modo){
    switch(modo){
        case GSM: 
            PIC_TX_to_TF_RX = 1;
            TF_TX_to_PIC_RX = 1;
            PIC_TX_to_SIM_RX = 0;
            SIM_TX_to_PIC_RX = 0;
            tipo_modo = GSM;
            break;
        case TF02: 
            PIC_TX_to_SIM_RX = 1;
            SIM_TX_to_PIC_RX = 1;
            PIC_TX_to_TF_RX = 0;
            TF_TX_to_PIC_RX = 0;
            tipo_modo = TF02;
            break;
    }
}