/* 
 * File:   UART.h
 * Author: EAVELLANEDA
 *
 * Created on 28 de septiembre de 2021, 19:13
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "config.h"
    
    void UART_Init(void);
    void UART_Begin(unsigned long baud);
    void UART_Write( char data);
    void UART_Println(char *buffer);
    void UART_Print(char *buffer); 

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

