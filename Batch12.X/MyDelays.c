#include "MyDelays.h"

/************************
 * Function delays 1ms  *
 * **********************/
void Delay_1ms(){
#if CLOCK == 4000000            //if clock is 4MHz
    register char r0;
    r0 = 0;
    while(r0 < 98)
        r0++;
    NOP();NOP();NOP();
#elif CLOCK == 8000000          //if clock is 8MHz
    register char r0;
    r0= 0;
    while(r0 < 197)
        r0++;
    NOP();NOP();NOP();NOP();NOP();NOP();
#else                           //if clock is 10MHz
    register char r0;
    r0= 0;
    while(r0 < 247)
        r0++;
    NOP();NOP();NOP();NOP();NOP();NOP();
#endif
}


/************************
 * Function delays 5ms  *
 * **********************/
void Delay_5ms(){
#if CLOCK == 4000000               //if clock is 4MHz
    register unsigned short r0;
    r0 =  0;
    while(r0 < 332){
            r0++;
        }
#elif CLOCK == 8000000               //if clock is 8MHz
    register unsigned short r0;
    r0 =  0;
    while(r0 < 664){
            r0++;
        }
#else                               //if clock is 10MHz
    register unsigned short r0;
    r0 =  0;
    while(r0 < 829)
            r0++;
#endif
}



/************************
 * Function delays 10ms  *
 * **********************/
void Delay_10ms(){
 #if CLOCK == 4000000               //if clock is 4MHz
    register unsigned short r0;
    r0 =  0;
    while(r0 < 665){
            r0++;
        }
#elif CLOCK == 8000000               //if clock is 8MHz
    register unsigned short r0;
    r0 =  0;
    while(r0 < 1332){
            r0++;
        }
#else                              //if clock is 10MHz
    register unsigned short r0;
    r0 =  0;
    while(r0 < 1670)
            r0++;
#endif

}


/**************************
 * Function delays 100ms  *
 * ***********************/
void Delay_100ms(){
#if CLOCK == 4000000                //if clock is 4MHz
    register unsigned short r0, r1;
    r1 = 0;
    while(r1 < 10){
        r0 =  0;
        while(r0 < 665)
                r0++;
        r1++;
    }
#elif CLOCK == 8000000              //if clock is 8MHz
    register unsigned short r0, r1;
    r1 = 0;
    while(r1 < 20){
        r0 =  0;
        while(r0 < 665)
                r0++;
        r1++;
    }
#else                               //if clock is 10MHz
    register unsigned short r0,r1;
    r1 = 0;
    while(r1 < 25){
        r0 =  0;
        while(r0 < 665)
                r0++;
        r1++;
    }
#endif
}

/***************************
 * Function delays 1000ms  *
 * ************************/
void Delay_1000ms(){
#if CLOCK == 4000000                //if clock is 4MHz
    register unsigned short r0;
    register char r1;
    r1 = 0;
    while(r1 < 2){
        r0 =  0;
        while(r0 < 33332)   //66665.92
                r0++;
        r1++;
    }
#elif CLOCK == 8000000              //if clock is 8MHz
    register unsigned short r0;
    register char r1;
    r1 = 0;
    while(r1 < 4){
        r0 =  0;
        while(r0 < 33335)
                r0++;
        r1++;
    }
#else                               //if clock is 10MHz
    register unsigned short r0;
    register char r1;
    r1 = 0;
    while(r1 < 5){
        r0 =  0;
        while(r0 < 33335)
                r0++;
        r1++;
    }
#endif
}


/***************************
 * Function delays    5us  *
 * ************************/
void Delay_5us(){
#if CLOCK == 4000000
    NOP(); NOP(); NOP(); NOP();NOP();
#elif CLOCK == 8000000
    NOP(); NOP(); NOP(); NOP();NOP();
    NOP(); NOP(); NOP(); NOP();NOP();
#elif CLOCK == 10000000
    NOP(); NOP(); NOP(); NOP();NOP();
    NOP(); NOP(); NOP(); NOP();NOP();
    NOP(); NOP(); NOP();
#endif
}

void Delay_50us(){
    register char r0;
    r0 = 0;
    while(r0 < 3){
#if CLOCK == 4000000
    NOP(); NOP(); NOP(); NOP();NOP();
#elif CLOCK == 8000000
    NOP(); NOP(); NOP(); NOP();NOP();
    NOP(); NOP(); NOP(); NOP();NOP();
#elif CLOCK == 10000000
    NOP(); NOP(); NOP(); NOP();NOP();
    NOP(); NOP(); NOP(); NOP();NOP();
    NOP(); NOP(); NOP();
#endif
        r0++;
    }
}