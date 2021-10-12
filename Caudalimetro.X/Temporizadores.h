/* 
 * File:   Temporizadores.h
 * Author: EAVELLANEDA
 *
 * Created on 28 de septiembre de 2021, 15:59
 */

#ifndef TEMPORIZADORES_H
#define	TEMPORIZADORES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "config.h"
#define T1_ON   T1CONbits.TMR1ON = 1
#define T1_OFF  T1CONbits.TMR1ON = 0
#define T0_ON   T0CONbits.TMR0ON = 1
#define T0_OFF  T0CONbits.TMR0ON = 0
  
    void Timer0_Init(unsigned int tiempo);
    void Timer0_Set(unsigned int tiempo);
    void Timer1_Init(unsigned int tiempo);
    void Timer1_Set(unsigned int tiempo);

#ifdef	__cplusplus
}
#endif

#endif	/* TEMPORIZADORES_H */

