#include <p16F1829.inc>
 __CONFIG _CONFIG1, (_FOSC_INTOSC & _WDTE_OFF & _PWRTE_OFF & _MCLRE_OFF &
_CP_OFF & _CPD_OFF & _BOREN_ON & _CLKOUTEN_OFF & _IESO_OFF & _FCMEN_OFF);
 __CONFIG _CONFIG2, (_WRT_OFF & _PLLEN_OFF & _STVREN_OFF & _LVP_OFF);
 errorlevel -302 ;supress the 'not in bank0' warning
 ORG 0
Start:
 banksel TRISC ; select bank1
 bcf TRISC,0 ; make IO Pin C0 an output
 banksel LATC ; select bank2
 clrf LATC ; init the LATCH by turning off everything
 bsf LATC,0 ; turn on LED C0 (DS1)
 goto $ ; sit here forever!
 end
;