/*
 * File:   newmain.c
 * Author: Heshan
 *
 * Created on May 3, 2014, 9:15 AM
 */
#include "LCD_Library.h"


void initialize_LCD(){
    //configure data pins
    LCD_DBIT_4_DIRECTION = 0; LCD_DBIT_5_DIRECTION = 0; LCD_DBIT_6_DIRECTION = 0; LCD_DBIT_7_DIRECTION = 0;
    LCD_DBIT_4 = 0; LCD_DBIT_5 = 0; LCD_DBIT_6 = 0; LCD_DBIT_7 = 0;
    
    //make controll pins 
    LCD_RS_DIRECTION  = 0; LCD_EN_DIRECTION = 0;

    #ifdef LCD_RW_DIRECTION
        LCD_RW_DIRECTION = 0;
    #endif

    LCD_RS = 0; LCD_EN = 0;
    #ifdef LCD_RW
        LCD_RW = 0;          //select instruction register to write
    #endif


    Delay_5ms();
    
    //configure the device
    LCD_DBIT_7 = 0; LCD_DBIT_6 = 0; LCD_DBIT_5 = 1; LCD_DBIT_4 = 1;
    LCD_EN = 1; NOP();NOP();NOP();NOP(); LCD_EN = 0;
    Delay_5ms();
    LCD_EN = 1; NOP();NOP();NOP();NOP(); LCD_EN = 0;
    Delay_1ms();
    LCD_EN = 1; NOP();NOP();NOP();NOP(); LCD_EN = 0;
    Delay_1ms();
   
    
    LCD_DBIT_4 = 0;
    LCD_EN = 1; Delay_5us(); LCD_EN = 0;
    Delay_5ms();
    
    //LCD_WriteCommand(40);
    //LCD_WriteCommand(16);
    //LCD_WriteCommand(1);
    //LCD_WriteCommand(15);
    LCD_WriteCommand(LCD_4BIT_MOD);     //4bit interface
    LCD_WriteCommand(LCD_ON);
    LCD_WriteCommand(LCD_CLEAR);
    LCD_WriteCommand(LCD_CURSOR_ON);
    LCD_WriteCommand(LCD_BLINK_OFF);
}

char LCD_deviceReady(){
    LCD_DBIT_7_DIRECTION = 1;
    LCD_RS = 0;
    #ifdef LCD_RW
        LCD_RW = 1;          
    #endif

    LCD_EN  = 1; Delay_5us(); LCD_EN = 0;

    if( LCD_DBIT_7 == 1){           //return 0 if device is busy
        LCD_DBIT_7_DIRECTION = 0;
        return 1;
    }else{
        LCD_DBIT_7_DIRECTION = 0;         //return 1 if device ready
        return 0;
    }

}

void LCD_WriteCommand(char cmd){
    LCD_EN = 0;
    LCD_RS = 0;
    
#ifdef LCD_RW
        LCD_RW = 0;          //select instruction register to write
#endif

    LCD_DBIT_7 = (cmd & 0b10000000)>>7; LCD_DBIT_6 = (cmd & 0b01000000)>>6;
    LCD_DBIT_5 = (cmd & 0b00100000)>>5; LCD_DBIT_4 = (cmd & 0b00010000)>>4;
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    LCD_EN = 1; Delay_5us(); LCD_EN = 0;
    

    LCD_DBIT_7 = (cmd & 0b00001000)>>3; LCD_DBIT_6 = (cmd & 0b00000100)>>2;
    LCD_DBIT_5 = (cmd & 0b00000010)>>1; LCD_DBIT_4 = (cmd & 0b00000001);
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    LCD_EN = 1; Delay_5us(); LCD_EN = 0;
    Delay_5ms();

}

void LCD_WriteChar(char data){
    LCD_EN = 0;
    LCD_RS = 1;             //select data register to write
#ifdef LCD_RW
        LCD_RW = 0;          //select instruction register to write
#endif
    LCD_DBIT_7 = (data & 0b10000000)>>7; LCD_DBIT_6 = (data & 0b01000000)>>6;
    LCD_DBIT_5 = (data & 0b00100000)>>5; LCD_DBIT_4 = (data & 0b00010000)>>4;
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    LCD_EN = 1; Delay_5us(); LCD_EN = 0;


    LCD_DBIT_7 = (data & 0b00001000)>>3; LCD_DBIT_6 = (data & 0b00000100)>>2;
    LCD_DBIT_5 = (data & 0b00000010)>>1; LCD_DBIT_4 = (data & 0b00000001);
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    LCD_EN = 1; Delay_5us(); LCD_EN = 0;
    Delay_5ms();
}

void LCD_WriteCharAt(char row, char column, char chr){
    unsigned char register  location;
    switch(row){
    case 1: location = 127 + column; break;
    case 2: location = 191 + column; break;
    case 3: location = 147 + column; break;
    case 4: location = 211 + column; break;
    default: break;}
    LCD_WriteCommand(location);
    LCD_WriteChar(chr);
}

void LCD_WriteString(char* string){
    while(*string)                // Write data to LCD up to null
         {
          LCD_WriteChar(*string);
          string++;               // Increment buffer
         }
    return;
}

void LCD_WriteStringAt(char row, char column, char* string){
    unsigned char data;
    switch(row){
    case 1: data = 127 + column; break;
    case 2: data = 191 + column; break;
    case 3: data = 147 + column; break;
    case 4: data = 211 + column; break;
    default: break;}

    LCD_WriteCommand(data);

    while(*string)                // Write data to LCD up to null
         {
          LCD_WriteChar(*string);
          string++;               // Increment buffer
         }
    return;
}


