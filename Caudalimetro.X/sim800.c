#include "sim800.h"
#include "UART.h"   

char *IP = "29.10.12.14";
char *PORT = "131";
char *APN = "claro.pe";

void Socket_Init(int proceso){
    //Activacion GPRS
    if(proceso == 1){
        UART_Println("AT+CGATT?");
    }
    
    //APN
    if(proceso == 2){
        UART_Print("AT+CSTT=\"");
        UART_Print(APN);
        UART_Println("\"");
    }
    //GPRS como CSD
    if(proceso == 3){
        UART_Println("CIICR");
    }
    //Inicio de conexión
    if(proceso == 4){
        UART_Print("AT+CIPSTART=\"UDP\",\"");
        UART_Print(IP);
        UART_Print("\",");
        UART_Print(PORT);
        UART_Println("\"");
    }
    //Mensaje
    if(proceso == 5){
        UART_Println("AT+CIPSEND");
        UART_Print("Test_comunicacion");
        UART_Write(0x1A);
    }
    
    //Cierre de comunicacion
    if(proceso == 6){
        UART_Println("AT+CIPCLOSE");
    }
}