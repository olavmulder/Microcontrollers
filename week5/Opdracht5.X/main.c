/*
 * File:   main.c
 * Author: olav
 *
 * Created on December 1, 2020, 9:43 AM
 * 
 * PR2 module voor increment = periode, regelen resolutie
 *      255 stappen = pr2 op 63
 * prescaler 4
 * 50% duty cycle = teller(CCPR2L:CCP2CON<5:4> in op zelfd 4*PR2+1) = 512
 */

/*
 *timer regelt owm periode. = 8 bit, wordt geregeld door PR2 macht register
 * PR2 aantal stappen mer periode TMR2 PreScaler staptijd regelen
 * 
 * tijd periode; TMR2 =0 tot TMR2 = PR2(met eventuele prescaler)
 * PulseWidth; TMR2 = CCPR2H:CCP2CON<5:4>
 * ------------------
 * PR2  =63 = 8Bits
 * Pwm period = 1/60
 * prescalerer 4
 */
#include "config.h"
#include "values.h"
#define BUTTON PORTAbits.RA2
#define _XTAL_FREQ 500000
#define LED LATCbits.LATC3
#define START_TIMER0 131
enum{
    ON,
    OFF      
}bitstate;

unsigned char dimrichting=1;
unsigned char counter=0;
void PWM2_Set_Duty(uint16_t DC);
void HandleButton();
void init(void);
void init_timer(void);
void init_interrupt(void);
void main(void) {
    init();
    init_timer();
    init_interrupt();
    while(1)continue;
}
void PWM2_Set_Duty(uint16_t counter)
{
  if(counter<256)
  {
    unsigned char DC = (unsigned char)lookupTable[counter];
    CCPR2L = (unsigned char)DC ;
    
  }
}
void HandleButton(void){
    __delay_ms(15);
    if(dimrichting == 1){
        dimrichting=0;
    }
    else{
        dimrichting = 1;
    }
    while(!BUTTON){
        if(dimrichting == 1 && counter < 255 ){
            counter ++;
            PWM2_Set_Duty(counter);
        }else if(dimrichting ==0 && counter > 0){
            counter --;
            PWM2_Set_Duty(counter);
        }
        TMR0 = START_TIMER0;
        TMR0IE = 1;
        TMR0IF =0;
        while(!TMR0IF)continue;
       
          
    }
}
void init(void){
    TRISA   =   0xFF;       
    TRISB   =   0x00;
    LATB    =   0x00;
    TRISC   =   0x00;
    ANSELA  =   0x00;
    OSCCON  =   0b00111000;
    LED     =   ON;
}
void init_timer(void){
    CCP2CON = 0b00001100;
    T2CON =   0b00000110;//prescaler 16 = ~122Hz;
    CCPTMRS = 0b00000000;//select timer 2 pwm module;
    PR2 = 63;//8 bit resolutie
    T2CKPS0 = 0;
    T2CKPS1 = 1;
    TMR2ON = 1;
    OPTION_REG = 0b00000100;
}
void init_interrupt(void){
    GIE = 1;
    PEIE =1;
    TMR0IE =0 ;
    IOCIE = 1;
    INTEDG = 1;//falling edge
    IOCAN2 = 1;//pin A2
}
void __interrupt() ISR(void){
    
    if(TMR0IF){
        TMR0IF = 0;
    }
    if(IOCAFbits.IOCAF2 == 1){
        HandleButton();
        IOCAFbits.IOCAF2 = 0;
    }
}
