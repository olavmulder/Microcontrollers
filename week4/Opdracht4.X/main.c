/*
 * File:   main.c
 * Author: olav
 *
 * Created on November 12, 2020, 10:57 AM
 */

#include "config.h"
#define _XTAL_FREQ 500000
#define MOTOR_PLUS LATCbits.LATC0
#define MOTOR_MIN  LATCbits.LATC1
#define LED LATCbits.LATC2

void init(void);
void init_timer(void);
void __interrupt() ISR(void);

volatile unsigned char add(volatile unsigned char, volatile unsigned char);
volatile unsigned char addISR(volatile unsigned char, volatile unsigned char);
void main(void){
    TRISC =         0x00;
    LATC =          0x00;
    INTCON =        0b11100000;
    OSCCON =        0b00111000;
    OPTION_REG =    0b10000111;
    TMR0 = 0;
   
    while(1){
        __delay_ms(5000);
        LATC = add(0, 0);     
    }
    
}
volatile unsigned char add(volatile unsigned char add, volatile unsigned char reg){
    return reg+add;
}
volatile unsigned char addISR(volatile unsigned char add, volatile unsigned char reg){
    return reg+add;
}    
void __interrupt() ISR(){
    if(LATC > 15){
        LATC = 0;
    }
    LATC = addISR(1, LATC);
    TMR0IF = 0;
    TMR0=0;
}
