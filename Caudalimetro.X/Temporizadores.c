#include "Temporizadores.h"

void Timer0_Init(void){
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS  = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS = 0b011;
    TMR0 = 0;
    T0CONbits.TMR0ON = 1;
}

void Timer1_Init(void){
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1;
    T1CONbits.RD16 = 1;
    T1CONbits.T1RUN = 0;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.TMR1CS = 0;
    T1CONbits.T1CKPS = 0b11;
    T1CONbits.TMR1ON = 1;
}

void Timer2_Init(void){
    T2CONbits.T2CKPS = 0b10;
    T2CONbits.TOUTPS = 0b1111;
    PR2 = 250;
    T2CONbits.TMR2ON = 1;
    PIR1bits.TMR2IF = 0;
    PIE1bits.TMR2IE = 1;
}
