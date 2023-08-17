/* 
 * File:   main.c
 * Author: olavm
 *
 * Created on November 3, 2020, 9:58 AM
 */


#include "config.h"

#define BUTTON PORTAbits.RA2

#define ON    0
#define OFF   1

#define NON 0 
#define ONE 1
#define TWO 2

#define _XTAL_FREQ 500000

void init(void);
void inleermodus(unsigned char *array, short int *scale_array, char* double_click);
void weergavemodus(unsigned char *array, char* double_click);
void check_click(char* double_click);

enum stages{
    a = 15,
    b = 7,
    c = 3,
    d = 1,
    e = 0,
}stages;

int main(void) {
    unsigned char array[10];
    short int     scale_array[5];
    char  double_click = NON;
    char* ptr;
    ptr = &double_click;
    init();
    
    for(int i=0;i<5;i++){
        scale_array[i] = (1024/5)*i;
    }
    while(1){
        inleermodus(array, scale_array, ptr);
        weergavemodus(array, ptr);
    }
    return (EXIT_SUCCESS);
}

void init(void){
    OSCCON = 0b00111000;
    TRISC = 0x00;//output
    TRISA = 0xff;//input
    ANSELAbits.ANSA4 = 1;
    ANSELAbits.ANSA2 = 0;
    LATC = 0xFF;//all off
    ADCON0 = 0b00001101;
    ADCON1 = 0b10010000;
    
}
void weergavemodus(unsigned char *array, char* double_click){
    *double_click = NON;
    while (*double_click != TWO){
        for(int i=0;i<10 && *double_click != TWO ;i++){
            LATC = array[i];
            //__delay_ms(500);
            for(int j=0;j<5&& *double_click != TWO ;j++){
                for(int k=0;k<255 && *double_click != TWO ;k++){
                    check_click(double_click);
                }
            }
            
        }
    }
    LATC = 0xFF;
   
}
void inleermodus(unsigned char *array, short int *scale_array, char* double_click){
    *double_click = NON;
    unsigned char indicator = 0;
    char offset = 15;
    unsigned char *array_base = &array[0];
    while(*double_click != TWO){
        __delay_us(5);
        GO = 1;
        while(GO)continue;
        check_click(double_click);
        if(*double_click == ONE){
            if(ADRES >scale_array[4] + offset){
                stages = e;
            }
            else if(ADRES > scale_array[3] + offset && ADRES < scale_array[4] - offset ){
                stages = d;
            }
            else if(ADRES > scale_array[2] + offset && ADRES < scale_array[3] - offset ){
                stages = c;
            }
            else if(ADRES > scale_array[1] + offset && ADRES < scale_array[2] - offset){
                stages = b;            
            }
            else if(ADRES >= scale_array[0] && ADRES < scale_array[1] - offset){
                stages = a;
            }
            array = (array_base+indicator);
            *array = stages;
            indicator+=1;
            while(indicator > 9 && *double_click != TWO){
               check_click(double_click);
            }
        }
    }
}

void check_click(char* double_click){
    *double_click = NON;
    if(BUTTON == ON){
        __delay_ms(10);
        if(BUTTON == ON){
            *double_click = ONE;
            while(BUTTON == ON)continue;
            for(int i=0;i<10 && *double_click != TWO;i++){
                for(int j=0;j<255 && *double_click != TWO;j++){
                    if(BUTTON == ON){
                        *double_click = TWO;
                        while(BUTTON == ON)continue;  
                    }
                }
            }
        }
    }
}  