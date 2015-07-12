/* 
 * File:   LCD_Library.h
 * Author: Heshan
 *
 * Created on May 8, 2014, 4:12 PM
 */

#ifndef LCD_LIBRARY_H
#define	LCD_LIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include "MyDelays.h"

#define LCD_RS                  PORTDbits.RD2
#define LCD_RS_DIRECTION        TRISDbits.TRISD2

//#define LCD_RW                PORTCbits.RC1
//#define LCD_RW_DIRECTION      TRISCbits.TRISC1

#define LCD_EN                  PORTDbits.RD3
#define LCD_EN_DIRECTION        TRISDbits.TRISD3

#define LCD_DBIT_4              PORTDbits.RD4
#define LCD_DBIT_4_DIRECTION    TRISDbits.TRISD4

#define LCD_DBIT_5              PORTDbits.RD5
#define LCD_DBIT_5_DIRECTION    TRISDbits.TRISD5

#define LCD_DBIT_6              PORTDbits.RD6
#define LCD_DBIT_6_DIRECTION    TRISDbits.TRISD6

#define LCD_DBIT_7              PORTDbits.RD7
#define LCD_DBIT_7_DIRECTION    TRISDbits.TRISD7



/************************************
 *  Acceptable Commands list        *
 * **********************************/
//Display and cursor settings
#define LCD_CLEAR               0x01
#define LCD_MOVE_HOME           0x02        //move the cursor to home
#define LCD_ON                  0x0C
#define LCD_OFF                 0x08
#define LCD_CURSOR_ON           0x0E        //turn on the cursor
#define LCD_CURSOR_OFF          0x0C        //turn off the cursor
#define LCD_BLINK_ON            0x0D        //0b00001110  //cursor blink on
#define LCD_BLINK_OFF           0x0C        //0b00001111  //cursor blink off
//Display and cursor movements
#define LCD_SHIFT_SCREEN_LEFT   0x1B        //0b00011011
#define LCD_SHIFT_SCREEN_RIGHT  0x1F        //0b00011111
#define LCD_SHIFT_CURSOR_LEFT   0x13        //0b00010011
#define LCD_SHIFT_CURSOR_RIGHT  0x17        //0b00010111
#define LCD_FIRST_LINE          0x00
#define LCD_SECOND_LINE         0x40
#define LCD_THIRD_LINE          0x14
#define LCD_FOURTH_LINE         0x54
//Interface and basic settings
#define LCD_8BIT_MOD            0x38        //8 bit interface, 2lines, 5x8 pixels
#define LCD_4BIT_MOD            0x28        //4 bit interface, 2lines, 5x8 pixels
/*************************************/




void initialize_LCD(void);
void LCD_WriteCommand(char cmd);
void LCD_WriteChar(char data);
void LCD_WriteCharAt(char row, char column, char data);
void LCD_WriteString(char *string);
void LCD_WriteStringAt(char row, char column, char *string);
char LCD_deviceReady(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_LIBRARY_H */

