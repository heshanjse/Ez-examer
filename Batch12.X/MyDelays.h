/* 
 * File:   MyDelays.h
 * Author: Heshan
 *
 * Created on May 6, 2014, 2:33 PM
 */

#ifndef MYDELAYS_H
#define	MYDELAYS_H

 #include <xc.h>

#ifndef CLOCK
   #define CLOCK  10000000
#endif

void Delay_1ms(void);
void Delay_5ms(void);
void Delay_10ms(void);
void Delay_100ms(void);
void Delay_1000ms(void);

void Delay_5us(void);
void Delay_50us(void);

#endif	/* MYDELAYS_H */

