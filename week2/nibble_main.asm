processor 16f1829
#include <xc.inc>
config FOSC=INTOSC, WDTE=OFF, PWRTE=OFF, MCLRE=OFF, CP=OFF, CPD=OFF
config BOREN=ON, CLKOUTEN=OFF, IESO=OFF, FCMEN=OFF
config WRT=OFF, PLLEN=OFF, STVREN=OFF, BORV=HI, LVP=OFF

psect reset_section,abs,class=CODE,delta=2
reset_vector:
    pagesel	start
    goto	start
psect start, class=CODE, delta=2
#define SWITCH	PORTA,2	;SWITCH is PORTA.2
ARRAY		EQU	0x70	;address for led array in shared ram
LOOP2SEC1	EQU	0x71
LOOP2SEC2	EQU	0x72
LOOP4SEC1	EQU	0x73
LOOP4SEC2	EQU	0x74
LOOP4SEC3	EQU	0x79
LOOPDEBOUNCE1	EQU	0x75
LOOPDEBOUNCE2	EQU	0x76
COUNTER		EQU	0x78
    //TMR0 register where timer value is saved
start:
   banksel ANSELA	;select anselA bank
    bcf	    ANSELA,2	;set ANCELA as digital input
    banksel TRISC	;select bankel tris
    clrf    TRISC	;clear TRISC, all pins are outputs
    bsf	    TRISA, 2	;set ra2 as input
    movlw 0b00111000	;500khz clock
    movwf   OSCCON	;
    banksel LATC	;select banksel latc
    movlw   0xFF	;
    movwf   LATC	;all led off exept 1
    banksel PORTA	;select bank porta(0)
    movlw   0x04
    movwf   COUNTER
    clrf    ARRAY
    //button 1 == los 0 == pushed
 //check if a2 is pushed
flashdelayinit:
    banksel LATC
    bcf	    LATC,0
    movlw   0xff
    movwf   0x120
    movlw   0xEE
    movwf   0x121
flashdelay:   
    decfsz  0x120
    bra	    flashdelay
    decfsz  0x121
    bra	    flashdelay
    bsf	    LATC,0
    banksel 0
initDelays:
    movlw   0xff
    movwf   LOOPDEBOUNCE1
    movlw   0x04
    movwf   LOOPDEBOUNCE2
    movlw   0xff	;2sec = (1/500000)*4 = 25000 instucties;
    ;256 *3 = 768 instructies
    movwf   LOOP2SEC1	;
    movlw   0x6D	;768+3 * 32 ~= 25000 
    movwf   LOOP2SEC2	;
/*debounceloop:
    decfsz  LOOPDEBOUNCE1
    bra	    debounceloop
    decfsz  LOOPDEBOUNCE2
    bra	    debounceloop*/
buttonpress1:
    btfsc   SWITCH
    bra	    buttonpress1
debounceloop:
    decfsz  LOOPDEBOUNCE1
    bra	    debounceloop
    decfsz  LOOPDEBOUNCE2
    bra	    debounceloop
released:
    btfss   SWITCH
    bra	    released
    incf    ARRAY,1	;add 1, 1x pushed
twosecloop:
    btfss   SWITCH		;if button pressed
    bra	    released2
    decfsz  LOOP2SEC1, f
    bra	    twosecloop
    decfsz  LOOP2SEC2, f
    bra	    twosecloop
    lslf    ARRAY,1
    decfsz  COUNTER, f
    goto    flashdelayinit
    goto    showAndDelay
released2:
    btfss   SWITCH
    bra	    released2
buttonpress2:
    decf    ARRAY,1
    lslf    ARRAY,1
    decfsz  COUNTER
    goto    flashdelayinit
showAndDelay:
    lsrf    ARRAY,1	;right shit array 
    movlw   0xff	; move 255 in loop4sec1
    movwf   LOOP4SEC1
    movlw   0xD5	;move 213 in loop4sec2
    movwf   LOOP4SEC2
    movlw   0x03	; move 3 in loop4sec4
    movwf   LOOP4SEC3
    banksel LATC	;banksel latc
    movf    ARRAY,0	;move array to w
    movwf   LATC	; move w to LATC
 Delay4sec:
    decfsz  LOOP4SEC1	; DELAY
    bra	    Delay4sec	;4 sec = 500000 instructions
    decfsz  LOOP4SEC2	; 256*3 = 768 (768+3)*(213+3)*3 = 499608 
    bra	    Delay4sec	;
    decfsz  LOOP4SEC3	;
    bra	    Delay4sec	;
    clrf    ARRAY	;clear ARRAY and counter
    clrf    COUNTER	;
    goto reset_vector	; go to reset vatctor
end reset_vector; defines the hex's entry point as `reset_vector'-label	

    
 
  