#include <stdio.h>
#include <stdlib.h>
#include <math.h>



#define MAXERROR 3.0
#define TOTALMULTI 4
#define TOTALCLOCK 7
#define TOTALBAUD  8
FILE *file;
typedef int baudrate;
typedef int clockspeed;
typedef int multiplier;
unsigned short compute_spbr( clockspeed c,  baudrate b,  multiplier m, double maxerrorpercentage);
int main(){
    
    baudrate b[] = {300,1200,2400,9600,10417,19200,57600,115200};
    clockspeed c[]= {5,10,20,40,80,160,320};
    multiplier m[]= {64,16,16,4};

     unsigned short S;
    file = fopen("array.txt", "w");
    fprintf(file, "const unsigned int ROM_GLOB_SPBRGLOOKUP[NUMSPEED][NUMBAUD][NUMMULTI]={\n");
    for(int i=0;i<TOTALCLOCK;i++){
        fprintf(file,"{\n");
        for(int j=0;j< TOTALBAUD;j++){
            fprintf(file,"{");
            for(int k=0;k<TOTALMULTI;k++){
               
                if(k>1){
                     S  = compute_spbr(c[i],b[j],m[k], MAXERROR);
                    fprintf(file, "%d", S);
                }else{
                    S = compute_spbr(c[i], b[j], m[k], MAXERROR);
                    if(S > 255){
                        fprintf(file, "0");
                    }else{
                        fprintf(file, "%d", S);
                    }
                }
               

                if(k +1 != TOTALMULTI){
                    fprintf(file, ",");
                }
            }
            fprintf(file,"},\n");
        }
        fprintf(file,"},\n");
    }
    fprintf(file,"}\n");

    
    return 0;
}
unsigned short compute_spbr( clockspeed c,  baudrate b,  multiplier m, double maxerror){
    unsigned short spBRG = (unsigned short)round((((c*100000.0)/b)/m)-1);  
    double BaudRate = round((c*100000.0)/(m * (spBRG+1)));
    double err = ((BaudRate - b)/b)* 100;
    
    if(fabs(err) < maxerror){
        return spBRG;
    }else{
        return 0;
    }
}   