/*
 * File:   main.c
 * Author: olav
 *
 * Created on December 16, 2020, 1:54 PM
 */


/**

 */
#include <pic16f1829.h>

#include "pic_uart.h"
#include <stdint.h>
#include "config.h"
#define _XTAL_FREQ 1000000

void init(void);

void main(void) {
    init();
    baudrate b = b9600;
    clockspeed c = c1;
    
    bool initb =  uart_init(b,c);
    if(initb == false){
       
        LATCbits.LATC0 = 1;
        
    }else{
        unsigned char readValue;
        while(1){
            
            while (EMPTY) continue;
            while(uart_numof_bytes_in_buffer() > 0){
                readValue = uart_read_char();
                uart_write_char(readValue);
            }
            uart_write_newline(); 
        }
    }
        
    
}
void init(void){
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 0;
    TRISCbits.TRISC5 = 1;  
   
    ANSELC = 0x00;
    TRISAbits.TRISA2 = 1;
    ANSELAbits.ANSA2 = 0; 
    
    //interrupt push button;
    IOCIE = 1;
    IOCIF = 0;
    IOCAN2 = 1;
    IOCAF2 = 0;
    LATC = 0x00;
}
void __interrupt()ISR(void){
    
    
    if(IOCAF2){
      
        __delay_ms(20);
        while(LATAbits.LATA2);
       
        const unsigned char stringToSend[] = "Button is pushed on the pic16f1829..Let's Goooooooo\r\n";
        uart_write_string(stringToSend);
       
        IOCAF2 = 0;   
        IOCIF = 0;
        
    }
    if(RCIF){
        LATCbits.LATC3 = ~LATCbits.LATC3;
        if(RCSTAbits.FERR){
            LATCbits.LATC2 = 1;//framing error;
            SPEN = 0;
            SPEN = 1;
        }
        if(OERR){
            LATCbits.LATC1 =1;
            CREN = 0;
            NOP();
            CREN = 1;
        }
        uart_handle_rcv_int();
    }
    
    
     
}