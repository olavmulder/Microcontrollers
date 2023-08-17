/*
 * File:   spiDisplay.c
 * Author: olav
 *
 * Created on December 9, 2020, 1:26 PM
 */
#define PROJECT


#include "config.h"
#include "ASCII.h"

//display
#define RESET1  PORTCbits.RC2
#define CE1     PORTCbits.RC4
#define BLANK1  PORTCbits.RC6
#define RS1     PORTCbits.RC7
 
#define RESET2  PORTDbits.RD2
#define CE2     PORTDbits.RD4
#define BLANK2  PORTDbits.RD6
#define RS2     PORTDbits.RD7


#define MAX_LENGHT 8
//adc
#define RESOLUTION 1024
#define VOLTAGE 5
#define STEP (RESOLUTION/VOLTAGE)
#ifdef PROJECT
enum bitstates{
    ON,
    OFF
}bitstate;
#else
enum bitstates{
    OFF,
    ON
}bitstate;
#endif
unsigned char adc_valueOld =0 ;
unsigned char volume = 0; //input Value from Rotery encoder;

void init(void);
//adc:
unsigned int get_adc_value(void);
unsigned char pot(void);
//display
void DotRegister(unsigned char *array, unsigned char index);
void ControlRegisterAll( uint8_t data,  unsigned char index);
void ControlRegister( unsigned char data,  unsigned char index);
void AssignTextToArray(unsigned char *input, unsigned char  *vol);
void AssignNumberToArray(unsigned char *array, unsigned char, unsigned char);
void fromValueToDigit(unsigned char volume, unsigned char *volArray);
//rotery
void update_rotation(unsigned char condition);

void main(void) {
    
    init();
    
    unsigned char volumeValue[2], *ptrVolArray; //value from pot meter;
    ptrVolArray = &volumeValue[0];
    
    //init array's;
    unsigned char input[40];
    unsigned char *pInput = NULL;
    pInput = &input[0];
   
    unsigned char vol[40];
    unsigned char *pVol = NULL;
    pVol = &vol[0];
    //assign default string: 'input' and 'vol.'
    unsigned char k=0;
    for(unsigned char i=0;i<7;i++){
        for(unsigned char j=0;j<5;j++){
            input[k] = ASCIITable[10+i][j];
            k++;
        } 
    }
    for(unsigned char j=0;j<5;j++){
        input[35+j] = ASCIITable[0][j];
    }
    k=0;
    for(unsigned char i=0;i<6;i++){
        for(unsigned char j=0;j<5;j++){
            if(i >=4){
                vol[k] = ASCIITable[16][j];
            }else{
                vol[k] = ASCIITable[17+i][j];
            }
            k++;
        }
    }
    
    while(1){
        volume = pot();

        AssignNumberToArray(pInput, 2, 0xff);
        
        fromValueToDigit(volume, ptrVolArray);
        AssignNumberToArray(pVol,volumeValue[0], volumeValue[1]);           
        DotRegister( pVol, 1);  
        DotRegister( pInput, 0);
    }
    
}
void init(){
    //adc
    ADCON0 = 0b10011101;//11 channel 7, 111 AN7, not start, on;
    ADCON1 = 0b10000000;//right vdd, vcc
    TRISE = 0xFF; //input
    TRISA = 0x00; //output
    //PORTA = channel;
    ANSELbits.ANS7 = 0x1; //analoge input
    
    //rotery:
    OSCCON = 0b01100111;
    TRISB =  0b00110000;
    ANSELH = 0b00000000;

    //display
    TRISC  = 0x00;
    TRISD  = 0x00;
    PORTC  = 0x00;
    PORTD  = 0x00;
    PORTB = 0x00;
    //SPI
    SSPEN = 0;
    SSPSTAT = 0b01000000;// data transmitted rising edge
    SSPCON =  0b00000010;//FOSC 4 = 0000, FOSC/16=0001 FOSC/64 = 0010
    SSPEN = 1;
   
    RESET1 = 0;
    RESET2 = 0;

    __delay_ms(10);
    
    RESET1 = 1;
    RESET2 = 1;
    unsigned char x[40];
    for(uint8_t i=0;i<40;i++){
        x[i] = 0x00; 
    }
    unsigned char *ptrX = &x[0];
    DotRegister(ptrX, 0);
    ControlRegisterAll(0b01111111, 0);
    __delay_ms(10);
    DotRegister(ptrX, 1);
    ControlRegisterAll(0b01111111, 1);
}

unsigned char pot(void){ 
      
    volatile unsigned int adc_value;
    unsigned char output = 0, jitter=20;
    
    adc_value = get_adc_value();

    for(uint8_t i=0;i < 4;i++){
        if(adc_value > STEP*(i+1) + jitter){
            bitstate = ON;
        }else if(adc_value < STEP*(i+1) - jitter){
            bitstate = OFF;
        }
        output |= (bitstate<<i);
    }
  
    PORTA = output;
    if(adc_value < 20){
        return 0;
    }
    if(adc_value > adc_valueOld + jitter ||  adc_value < adc_valueOld - jitter){
       adc_valueOld = (uint8_t)adc_value;
       return VolumeLookUp[adc_value];
    }else{
        return 0xff;
    }
 
}
unsigned int get_adc_value(){
    __delay_us(5);
    GO = 1;
    while(GO);
    return((unsigned int)(ADRESH<<8)+(unsigned int)ADRESL);
}

void fromValueToDigit(unsigned char volume, unsigned char *volArray){
    if(volume < 10){
        *volArray = 0;
        *(volArray+1) = volume;
    }else if(volume < 20){
        *volArray = 1;
        *(volArray+1) = (volume-10 > 9) ? 0 : volume-10;
    }
    
}
 

void DotRegister(unsigned char *array, unsigned char index){
    if(index){
        RS1= 0;
        __delay_ms(5);
        CE1 = 0;

        unsigned char x;
        for(uint8_t i=0;i<40;i++){

            SSPBUF= *(array+i);//data; 
            while (SSPSTATbits.BF==0);  //wait for transmission complete 
            x=SSPBUF;  //dummy read 

        }
        CE1 = 1;
    }else{
        RS2= 0;
        __delay_ms(5);
        CE2 = 0;

        unsigned char x;
        for(uint8_t i=0;i<40;i++){
            SSPBUF= *(array+i);//data; 
            while (SSPSTATbits.BF==0);  //wait for transmission complete 
            x=SSPBUF;  //dummy read 
        }
        CE2 = 1;
    }
}
void ControlRegisterAll(uint8_t data, unsigned char index){
    for (int i = 0; i < 2; i++) {
        ControlRegister(0b10000001, index);
    }
    ControlRegister(data, index);
    ControlRegister(0b10000000, index);
    
}
void ControlRegister(unsigned char data, unsigned char index){
    if(index){
        RS1 = 1;
        __delay_ms(5);
        CE1 = 0;     
        unsigned char x;
        SSPBUF=data;//data; 
        while (SSPSTATbits.BF==0);  //wait for transmission complete 
        x=SSPBUF;  //dummy read 
        CE1  = 1;
        __delay_ms(5);
    }else{
        RS2 = 1;
        __delay_ms(5);
        CE2 = 0;     
        unsigned char x;
        SSPBUF=data;//data; 
        while (SSPSTATbits.BF==0);  //wait for transmission complete 
        x=SSPBUF;  //dummy read 
        CE2  = 1;
        __delay_ms(5);      
    }
}
void AssignNumberToArray(unsigned char *array, unsigned char num1, unsigned char num2){
    unsigned char dum = 0xff;
    if(num2 == dum ){
        for(unsigned char j=0;j<5;j++){//display 0 = 1 number
            *(array+35+j) = ASCIITable[num1][j];
        }
        
    }else{
        for(unsigned char j=0;j<5;j++){
            *(array+30+j) = ASCIITable[num1][j];
        }
        for(unsigned char j=0;j<5;j++){
            *(array+35+j) = ASCIITable[num2][j];
        }
    }
    
}

 