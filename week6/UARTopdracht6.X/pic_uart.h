#ifndef PIC_UART_H
#define	PIC_UART_H

// Maak een aantal `named constants' voor:
// #define je bufferomvang
#define BUFFER 50
// #define de lege-buffer-afspraak
#define EMPTY (front == end)
// #define het aantal F_OSC-kloksnelheden
#define TOTALCLOCK 7
// #define het aantal UART-baudrates
#define TOTALBAUD 8
// #define het aantal UART-multipliers
#define TOTALMULTI 4

// Maak een aantal nieuwe datatypen voor:
// > Definieer een datatype `bool' (boolean)
    /*include "<bool.h>"*/
#include <stdbool.h>
// > Definieer een datatype `clockspeed' waarachter de gesloten set 
//   kloksnelheden uit de opdrachtkaders schuilgaat
typedef enum {
    b300, 
    b1200,   
    b2400,    
    b9600,  
    b10417,   
    b19200,  
    b57600,  
    b115200
}baudrate;
// > Definieer een datatype `baudrate' waarachter de gesloten set 
//	 baudrates uit de opdrachtkaders schuilgaat
typedef enum {
    c05,  
    c1  ,
    c2  ,
    c4  ,
    c8  ,
    c16 ,
    c32  
}clockspeed;

// > Definieer een datatype `multiplier' waarachter de gesloten set 
//   multipliers uit de opdrachtkaders schuilgaat
typedef enum{
    m64 ,
    m16 ,
    m162,
    m4  
}multiplier;

// Plak hier de 3D-look-up-table-array voor SPBRG-waarden die je op 
// de PC hebt gegenereerd. Controleer of de waarden kloppen door ze te 
// matchen met de waarden uit de PIC16F1829 datasheet, table 26-5, 
// pp. 307-309.
const unsigned int ROM_GLOB_SPBRGLOOKUP[TOTALCLOCK][TOTALBAUD][TOTALMULTI]=
{
{
    {25,103,103,416},
    {0,25,25,103},
    {0,12,12,51},
    {0,0,0,12},
    {0,2,2,11},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    },
{
    {51,207,207,832},
    {12,51,51,207},
    {0,25,25,103},
    {0,0,0,25},
    {0,5,5,23},
    {0,0,0,12},
    {0,0,0,0},
    {0,0,0,0},
    },
{
    {103,0,416,1666},
    {25,103,103,416},
    {12,51,51,207},
    {0,12,12,51},
    {2,11,11,47},
    {0,0,0,25},
    {0,0,0,0},
    {0,0,0,0},
},
    {
    {207,0,832,3332},
    {51,207,207,832},
    {25,103,103,416},
    {0,25,25,103},
    {5,23,23,95},
    {0,12,12,51},
    {0,0,0,16},
    {0,0,0,0},
},
{
    {0,0,1666,6666},
    {103,0,416,1666},
    {51,207,207,832},
    {12,51,51,207},
    {11,47,47,191},
    {0,25,25,103},
    {0,0,0,34},
    {0,0,0,16},
    },
{
    {0,0,3332,13332},
    {207,0,832,3332},
    {103,0,416,1666},
    {25,103,103,416},
    {23,95,95,383},
    {12,51,51,207},
    {0,16,16,68},
    {0,0,0,34},
},
{
    {0,0,6666,26666},
    {0,0,1666,6666},
    {207,0,832,3332},
    {51,207,207,832},
    {47,191,191,767},
    {25,103,103,416},
    {0,34,34,138},
    {0,16,16,68},
},
};
// Maak globale variabelen (shared memory) voor:
// > De UART-receive buffer (FIFO)
unsigned char buffer[BUFFER];
// > Een referentie naar of een index van het `front'-element van de 
//   buffer
unsigned char *front;

// > Een referentie naar of een index van het `end'-element van de 
//   buffer
unsigned char *end;
// In de functie uart_init():
// > Bufferhuishouding initialiseren;
// > Meest optimale SPBR-waarde bepalen;
// > UART-module configureren, inclusief receive interrupt.
bool uart_init(baudrate b, clockspeed c);
void uart_write_char(unsigned char byte);
void uart_write_string(const unsigned char *str);
// In de functie uart_write_newline():
// Schrijf achter elkaar de karakters '\r' and '\n' naar TX
void uart_write_newline(void);
// In de functie uart_write_newline():
// Een `line' eindigt met een `newline' :)
void uart_write_line(const unsigned char *str);
// In de functie uart_handle_rcv_int():
// Lees het karakter uit het receive register van de UART-module, 
// voeg het toe aan de FIFO en werk de bufferhuishouding bij.
void uart_handle_rcv_int(void);
// In de functie uart_read_char():
// Geef het voorste karakter (FRONT) uit de buffer terug en maak het
// volgende element in de buffer de nieuwe FRONT.
unsigned char uart_read_char(void);
unsigned char uart_numof_bytes_in_buffer(void);
// In de functie uart_flush_buffer():
// Maak de buffer leeg en breng hem weer in de lege begintoestand.
void uart_flush_buffer();

// LET OP: als je de `Looseness' van de `Coupling' 
// (\url{https://en.wikipedia.org/wiki/Loose_coupling}) van deze 
// functies wilt vergroten, dan is het beter om aan de functies die
// daarvoor in aanmerking komen de buffer als parameter mee te geven
// of vanuit de functie terug te geven (via return value of pointer-
// parameter). Je mag hierin zelf een keuze maken. De 
// functievoorschriften hierboven veronderstellen intern het bestaan
// en de aanspreekbaarheid van de globale buffer. Of dat de mooiste 
// keuze is, is betwijfelbaar. Een functie als:
//
// void uart_handle_rcv_int(void);
//
// zou je kennis kunnen laten nemen van het bestaan van de FIFO door
// hem te herschrijven als:
//
// void uart_handle_rcv_int(char *uart_rcv_fifo);
//
// Of moet het een unsigned char-pointer zijn? Denk hier zelf over na.
//
// Afspraak:
// =========
// Het `Looser' maken van de `Coupling' is de ENIGE omstandigheid
// waaronder je de signaturen van de funties mag aanpassen. Je blijft
// gehouden om deze routines als functionele eenheden in je 
// UART-library te realiseren!
void init_clock(clockspeed c);
#endif	/* PIC_UART_H */
