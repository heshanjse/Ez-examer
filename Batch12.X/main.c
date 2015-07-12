/*
 * File:   main.c
 * Author: Heshan //
 *
 * Created on May 8, 2014, 4:17 PM
 */


#include <xc.h>
#include "LCD_Library.h"
#include "MyDelays.h"
#include "plib/EEP.h"
#include "plib/adc.h"

#pragma config FOSC = HS         // Oscillator Selection bits (HS oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR  = OFF  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
//#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
//#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)


//key pad key definitions
#define UNDO_KEY    0x11
#define FLAG_KEY    0x12
#define MENU_KEY    0x14
#define LEFT_KEY    0x21
#define UP_KEY      0x22
#define RIGHT_KEY   0x24
#define DOWN_KEY    0x28
#define ANS_A_KEY   0x41
#define ANS_B_KEY   0x42
#define ANS_C_KEY   0x44
#define ANS_D_KEY   0x48
#define ANS_E_KEY   0x81

#define EEP_PAPER_START 0x0A            //START LOCATION OF PAPER IN EEPROM


//function prototypes
void initializeDevice(void);
unsigned char readKey(void);
void interruptConfig(char state);
void powerOnScreen(void);
char get_yes_no(char lcd_location);

void examSetup(void);
void setExamTime(void);
void setExamMCQ(void);
void startExam(void);
void runPaper(void);

void subMenu();

void getBeep(short cnt);
int viewflags();
int view_answers(int i, int j);
void saveAnswer(char qNumber, char *ans, char *flag);
void loadAnswer(char qNumber, char *ans, char *flag);

unsigned short mili_seconds = 0;
unsigned char hours = 0, minutes = 0, seconds = 0;
unsigned char timeFrame_cnt = 0;
unsigned char MODE;
char KEY_BUFFER;


unsigned char hourCnt = 1;
unsigned char minCnt = 30;
unsigned char MCQCnt = 60;

/***************************************
 * Enum holds the  mode of the display
 *  -MENU_MOD           = Admin Configuration mode
 *  -PAPER_MOD_SIMPLE   = Normal mode operation time is displyed
 *  -PAPER_MOD_COMPLEX  = Complex mode operation, time runs in the background
 *                        (for the menu mode during the exam)
 ***************************************/
enum timeMode {
    MENU_MOD, PAPER_MOD_SIMPLE, PAPER_MOD_COMPLEX
};
//enum define the acceptable answer

enum answer {
    A, B, C, D, E, MENU, FLAG
};

struct EE_Answer {
    char ans;
    char flag;
};

//GLOBAL VARIABLE STATE THE MENU MODE
enum timeMode GLOBLE_TMOD = MENU_MOD;

void main(void) {
    initializeDevice();
    examSetup();

}

/*******************************
 * Administrator conrol setup of
 * the device. Prompt three main
 * menu items to
 *  -configure time
 *  -configure MCQ number
 *  -Start the exam
 *******************************/
void examSetup() {
    interruptConfig(0); //shutdown interrupts
    char menuNumber = 1;
    char txt0[] = "********************";
    char txt1[] = "*     Welcome      *";
    char txt2[] = "*  Press menu key  *";

    char txt4[] = "Select an option";
    char txt5[] = " 1-Set time period";
    char txt6[] = " 2-Set questions";
    char txt7[] = " 3-Finalyze setup";



    //print the home screen
    LCD_WriteCommand(LCD_CLEAR);
    LCD_WriteStringAt(1, 1, txt0);
    LCD_WriteStringAt(2, 1, txt1);
    LCD_WriteStringAt(3, 1, txt2);
    LCD_WriteStringAt(4, 1, txt0);


    while (1) {
        if (readKey() == MENU_KEY) {
            while (1) {
                //print the main menu screen
                LCD_WriteCommand(LCD_CLEAR);
                LCD_WriteStringAt(1, 1, txt4);
                LCD_WriteStringAt(2, 3, txt5);
                LCD_WriteStringAt(3, 3, txt6);
                LCD_WriteStringAt(4, 3, txt7);
                LCD_WriteCharAt(menuNumber + 1, 1, 0b01111110);
                KEY_BUFFER = readKey();
                if (KEY_BUFFER == UP_KEY) {
                    LCD_WriteCharAt(menuNumber, 1, ' ');
                    if (menuNumber == 1) {
                        menuNumber = 3;
                    } else
                        menuNumber--;
                } else if (KEY_BUFFER == DOWN_KEY) {
                    LCD_WriteCharAt(menuNumber, 1, ' ');
                    if (menuNumber == 3) {
                        menuNumber = 1;
                    } else
                        menuNumber++;
                } else if (KEY_BUFFER == FLAG_KEY) {
                    if (menuNumber == 1) {
                        setExamTime();
                    } else if (menuNumber == 2) {
                        setExamMCQ();
                    } else if (menuNumber == 3) {
                        startExam();
                    }
                }
            }
        }
    }
    interruptConfig(1); //turn on interrupts
}//End of examSeup function

/******************************
 * Function reads time of
 * paper from the Administrator
 ******************************/
void setExamTime() {
    char txt1[] = "Set the exam time:";
    char txt1_2[] = "Hours     : "; //length 12
    char txt1_3[] = "Minutes   : ";

    //prmpt display
    LCD_WriteCommand(LCD_CLEAR);
    LCD_WriteStringAt(1, 1, txt1);
    LCD_WriteStringAt(2, 1, txt1_2);
    LCD_WriteStringAt(3, 1, txt1_3);

    //display the default exam time
    LCD_WriteCharAt(2, 13, 0x30 + hourCnt);
    LCD_WriteCharAt(3, 13, 0x30 + minCnt / 10);
    LCD_WriteCharAt(3, 14, 0x30 + minCnt % 10);
    LCD_WriteCharAt(2, 14, 01);
    LCD_WriteCommand(LCD_BLINK_ON);

    //read hours
    while (1) {
        LCD_WriteCharAt(2, 13, 0x30 + hourCnt);
        KEY_BUFFER = readKey();
        if (KEY_BUFFER == UP_KEY) {
            hourCnt = (hourCnt + 1) % 10;
        } else if (KEY_BUFFER == DOWN_KEY) {
            if (hourCnt == 0)
                hourCnt = 9;
            else
                hourCnt = (hourCnt - 1) % 10;
        } else if (KEY_BUFFER == FLAG_KEY) {
            break;
        }
    }

    //read minutes
    while (1) {
        LCD_WriteCharAt(3, 13, 0x30 + minCnt / 10);
        LCD_WriteCharAt(3, 14, 0x30 + minCnt % 10);

        KEY_BUFFER = readKey();
        if (KEY_BUFFER == UP_KEY) {
            minCnt = (minCnt + 1) % 60;
        } else if (KEY_BUFFER == DOWN_KEY) {
            if (minCnt == 0)
                minCnt = 59;
            else
                minCnt = (minCnt - 1) % 60;
        } else if (KEY_BUFFER == FLAG_KEY) {
            break;
        }
    }


    LCD_WriteCommand(LCD_BLINK_OFF);
    return;
}//End of setTime funtion

/***************************
 * Function reads number of
 * MCQ questions from the
 * Administrator
 ***************************/
void setExamMCQ() {
    char txt1_1[] = "Set the number of";
    char txt1_2[] = "MCQ questions: "; //length 15

    //prompt display
    LCD_WriteCommand(LCD_CLEAR);
    LCD_WriteStringAt(1, 1, txt1_1);
    LCD_WriteStringAt(2, 1, txt1_2);
    LCD_WriteCommand(LCD_BLINK_ON);

    //read question count
    while (1) {
        LCD_WriteCharAt(2, 16, 0x30 + MCQCnt / 10);
        LCD_WriteCharAt(2, 17, 0x30 + MCQCnt % 10);
        LCD_WriteCommand(LCD_BLINK_ON);

        KEY_BUFFER = readKey();
        if (KEY_BUFFER == UP_KEY) {
            MCQCnt = (MCQCnt + 1);
        } else if (KEY_BUFFER == DOWN_KEY) {
            if (MCQCnt == 0)
                MCQCnt = 0;
            else
                MCQCnt = (MCQCnt - 1);
        } else if (KEY_BUFFER == FLAG_KEY) {
            break;
        }
    }

    LCD_WriteCommand(LCD_BLINK_OFF);
}//End of setExamMCQ function

/***************************
 * Function initiate the
 * exam in simle view mode
 ***************************/
void startExam() {
    char txt1[] = "Are you sure?";

    LCD_WriteCommand(LCD_CLEAR);
    LCD_WriteStringAt(1, 1, txt1);

    if (get_yes_no(2)) {
        while (1) { //sleep message until user start exam
            LCD_WriteCommand(LCD_CLEAR);
            LCD_WriteStringAt(2, 3, "Press select to");
            LCD_WriteStringAt(3, 3, "Start the exam");
            KEY_BUFFER = readKey();
            if (KEY_BUFFER == FLAG_KEY) {
                getBeep(5);
                break;
            }
        }
        runPaper(); //start the paper
    }
}//End of startExam function

/*******************************
 * Function conduct the exam.
 *******************************/
void runPaper() {
    interruptConfig(1); //start the timer
    GLOBLE_TMOD = PAPER_MOD_SIMPLE;

    char qCount_tmp;
    struct EE_Answer ansBuffer;
    char txt1[] = "Question number: ";
    char txt2[] = "Your Answer:  ";



    LCD_WriteCommand(LCD_CLEAR);
    LCD_WriteCommand(LCD_CURSOR_OFF);


    //Start Question paper answering in normal mood
    while (1) {
        for (qCount_tmp = 1; qCount_tmp <= MCQCnt;) {
            interruptConfig(0);
            LCD_WriteStringAt(2, 1, txt1); //display question number
            LCD_WriteCharAt(2, 18, 0x30 + qCount_tmp / 10);
            LCD_WriteCharAt(2, 19, 0x30 + qCount_tmp % 10);
            LCD_WriteStringAt(3, 1, txt2); //prompt for answer

            //print the default answer or previous Answer
            if (ansBuffer.ans != 0) {
                LCD_WriteCharAt(3, 14, ansBuffer.ans);
            } else
                LCD_WriteCharAt(3, 14, '_');

            //print the flag state if available
            if (ansBuffer.flag == 1) {
                LCD_WriteCharAt(3, 20, 0b01111111); //0b11100010
            } else {
                LCD_WriteCharAt(3, 20, ' ');
            }

            interruptConfig(1);

            //get the answer from user
            KEY_BUFFER = readKey();
            if (KEY_BUFFER == LEFT_KEY) { //PREVIOUS QUESTION

                if (ansBuffer.ans == 0)
                    ansBuffer.flag = 1;

                //save the current answer                
                saveAnswer(qCount_tmp, &ansBuffer.ans, &ansBuffer.flag);

                if (qCount_tmp - 1 > 0) //DECREMENT THE QUESTION POINTER
                    qCount_tmp--;

                //load the previous answer
                loadAnswer(qCount_tmp, &ansBuffer.ans, &ansBuffer.flag);

            } else if (KEY_BUFFER == RIGHT_KEY) {//NEXT QUESTION
                //save the current answer
                saveAnswer(qCount_tmp, &ansBuffer.ans, &ansBuffer.flag);

                if (qCount_tmp + 1 <= 50)
                    qCount_tmp++;

                //load next buffered answer
                loadAnswer(qCount_tmp, &ansBuffer.ans, &ansBuffer.flag);

            } else if (KEY_BUFFER == MENU_KEY) {
                //save the current answer
                saveAnswer(qCount_tmp, &ansBuffer.ans, &ansBuffer.flag);
                //goto sub menu
                LCD_WriteCommand(LCD_CLEAR);
                GLOBLE_TMOD = PAPER_MOD_COMPLEX; //hide the time
                LCD_WriteStringAt(1, 1, "SUB MENU");
                Delay_100ms();
                subMenu();
                GLOBLE_TMOD = PAPER_MOD_SIMPLE; //view the time

            } else if (KEY_BUFFER == ANS_A_KEY) {
                ansBuffer.ans = 'A';
            } else if (KEY_BUFFER == ANS_B_KEY) {
                ansBuffer.ans = 'B';
            } else if (KEY_BUFFER == ANS_C_KEY) {
                ansBuffer.ans = 'C';
            } else if (KEY_BUFFER == ANS_D_KEY) {
                ansBuffer.ans = 'D';
            } else if (KEY_BUFFER == ANS_E_KEY) {
                ansBuffer.ans = 'E';
            } else if (KEY_BUFFER == FLAG_KEY) {
                ansBuffer.flag = 1;
            }


        }

        interruptConfig(0); //disable interrupts
        LCD_WriteCommand(LCD_CLEAR);
        LCD_WriteStringAt(1, 1, "You have finished");
        LCD_WriteStringAt(2, 1, "Submit answers");
        if (get_yes_no(3)) {
            LCD_WriteCommand(LCD_CLEAR);
            LCD_WriteStringAt(2, 1, "ok");
            Delay_1000ms();
        }
    }

}//End of runPaper function

void subMenu() {
    char menuNumber = 1;
    char txt0[] = "Select an option";
    char txt1[] = "1-view all answers";
    char txt2[] = "2-view all flags";
    char txt3[] = "3-Save and Submit";


    while (1) {
        //print the sub menu screen
        LCD_WriteCommand(LCD_CLEAR);
        LCD_WriteStringAt(1, 3, txt0);
        LCD_WriteStringAt(2, 3, txt1);
        LCD_WriteStringAt(3, 3, txt2);
        LCD_WriteStringAt(4, 3, txt3);
        LCD_WriteCharAt(menuNumber + 1, 1, 0b01111110);
        KEY_BUFFER = readKey();
        if (KEY_BUFFER == UP_KEY) {
            LCD_WriteCharAt(menuNumber, 1, ' ');
            if (menuNumber == 1) {
                menuNumber = 3;
            } else
                menuNumber--;
        } else if (KEY_BUFFER == DOWN_KEY) {
            LCD_WriteCharAt(menuNumber, 1, ' ');
            if (menuNumber == 3) {
                menuNumber = 1;
            } else
                menuNumber++;
        } else if (KEY_BUFFER == FLAG_KEY) {
            if (menuNumber == 1) {
                LCD_WriteCommand(LCD_CLEAR);
                view_answers(1, 1);
                //FUNCTION CALL FOR MENU ITEM 1
            } else if (menuNumber == 2) {
                LCD_WriteCommand(LCD_CLEAR);
                viewflags();
                //FUNCTION CALL FOR MENU ITEM 2
            } else if (menuNumber == 3) {
                LCD_WriteCommand(LCD_CLEAR);
                LCD_WriteStringAt(2, 5, "Connecting..");
                SLEEP(2000);
                //FUNCTION CALL FOR MENU ITEM 3
            }
        } else if (KEY_BUFFER == UNDO_KEY) {
            LCD_WriteCommand(LCD_CLEAR);
            return;
        }
    }
}

int viewflags() {
    int lc = 1;
    int rc = 2;
    for (int i = 1; i <= MCQCnt; i++) {
        if (eeprom_read(EEP_PAPER_START + (i)*2 - 1) == 1) {

            LCD_WriteCharAt(lc, rc, 48 + i / 10);
            LCD_WriteCharAt(lc, ++rc, 48 + i % 10);
            LCD_WriteCharAt(lc, ++rc, 0b00111010);
            LCD_WriteCharAt(lc, ++rc, eeprom_read(EEP_PAPER_START + (i - 1)*2));
            rc = rc + 3;
            if (rc == 20) {
                rc = 2;
                ++lc;
            }
        }

        while (lc == 5) {
            KEY_BUFFER = readKey();
            if (KEY_BUFFER == UP_KEY) {


            } else if (KEY_BUFFER == DOWN_KEY) {
                if (i != MCQCnt) {
                    LCD_WriteCommand(LCD_CLEAR);
                    lc = 1;
                }
            } else if (KEY_BUFFER == UNDO_KEY) {

                return 0;

            }
        }

    }
}

int view_answers(int i, int j) {
    int p;
    for (i = 1; i <= MCQCnt; i++) {
        p = 0;
        if (i % 3 == 1) {
            LCD_WriteCharAt(j, 2, 48 + i / 10);
            LCD_WriteCharAt(j, 3, 48 + i % 10);
            LCD_WriteCharAt(j, 4, 0b00111010);
            LCD_WriteCharAt(j, 5, eeprom_read(EEP_PAPER_START + (i - 1)*2));
            if (eeprom_read(EEP_PAPER_START + (i)*2 - 1) == 1) {
                LCD_WriteCharAt(j, 6, 0b01111111);
            }
        } else if (i % 3 == 2) {
            LCD_WriteCharAt(j, 8, 48 + i / 10);
            LCD_WriteCharAt(j, 9, 48 + i % 10);
            LCD_WriteCharAt(j, 10, 0b00111010);
            LCD_WriteCharAt(j, 11, eeprom_read(EEP_PAPER_START + (i - 1)*2));
            if (eeprom_read(EEP_PAPER_START + (i)*2 - 1) == 1) {
                LCD_WriteCharAt(j, 12, 0b01111111);
            }
        } else if (i % 3 == 0) {
            LCD_WriteCharAt(j, 14, 48 + i / 10);
            LCD_WriteCharAt(j, 15, 48 + i % 10);
            LCD_WriteCharAt(j, 16, 0b00111010);
            LCD_WriteCharAt(j, 17, eeprom_read(EEP_PAPER_START + (i - 1)*2));
            if (eeprom_read(EEP_PAPER_START + (i)*2 - 1) == 1) {
                LCD_WriteCharAt(j, 18, 0b01111111);
            }
            j++;
            if (i == 12 || i == 24 || i == 36 || i == 48 || i == 60) {
                p = 1;
                j = 1;
            }
        }
        while (p == 1) {
            KEY_BUFFER = readKey();
            if (KEY_BUFFER == UP_KEY) {

                if (i == 24) {
                    i = 0;
                    LCD_WriteCommand(LCD_CLEAR);
                    break;
                } else if (i == 36) {
                    i = 12;
                    LCD_WriteCommand(LCD_CLEAR);
                    break;
                } else if (i == 48) {
                    i = 24;
                    LCD_WriteCommand(LCD_CLEAR);
                    break;
                } else if (i == 60) {
                    i = 36;
                    LCD_WriteCommand(LCD_CLEAR);
                    break;
                }
            } else if (KEY_BUFFER == DOWN_KEY) {
                if (i != MCQCnt) {
                    LCD_WriteCommand(LCD_CLEAR);
                    break;
                }

            } else if (KEY_BUFFER == UNDO_KEY) {
                //  LCD_WriteCommand(LCD_CLEAR);
                return 0;

            }
        }
    }
}

void getBeep(short cnt) {
    for (; cnt > 0; cnt--) {
        //set the buzzer
        Delay_100ms();
        //off the buzzer
    }
}

//save the pointed answer in given question number slot in eeprom

void saveAnswer(char qNumber, char *ans, char *flag) {
    eeprom_write(EEP_PAPER_START + ((qNumber - 1)*2), *ans);
    eeprom_write(EEP_PAPER_START + (qNumber)*2 - 1, *flag);
}

//read the bufferd answer from eeprom and load to the answer pointer

void loadAnswer(char qNumber, char *ans, char *flag) {
    *ans = eeprom_read(EEP_PAPER_START + (qNumber - 1)*2);
    *flag = eeprom_read(EEP_PAPER_START + (qNumber)*2 - 1);
}

/***********************************
 * Function reads and return Answer
 ***********************************/
char readAnswer() {
    enum answer temp;


    while (1) {
        //read the answer
        KEY_BUFFER = readKey();
        LCD_WriteCommand(LCD_SHIFT_CURSOR_LEFT);
        LCD_WriteCommand(LCD_CURSOR_ON);
        if (KEY_BUFFER == ANS_A_KEY) {
            temp = A;
        } else if (KEY_BUFFER == ANS_B_KEY) {
            temp = B;
        } else if (KEY_BUFFER == ANS_C_KEY) {
            temp = C;
        } else if (KEY_BUFFER == ANS_D_KEY) {
            temp = D;
        } else if (KEY_BUFFER == ANS_E_KEY) {
            temp = E;
        } else if (KEY_BUFFER == FLAG_KEY) {
            temp = FLAG;
        } else if (KEY_BUFFER == MENU_KEY) {
            temp = MENU;
        }
        return temp;
    }

}

/*********************************
 * Function initialize the device
 *  -initialize lcd
 *  -setup interupts
 *  -setup ports
 **********************************/
void initializeDevice() {
    /*initialize portb for key pad
     * definitions of the acceptable key
     * key          patterns            hex code
     * UNDO     -   0001 0001           0x11
     * MENU     -   0001 0100           0x14
     * FLAG     -   0001 0010           0x12
     * LEFT     -   0010 0001           0x21
     * RIGHT    -   0010 0100           0x24
     * UP       -   0010 0010           0x22
     * DOWN     -   0010 1000           0x28
     * ANS_A    -   0100 0001           0x41
     * ANS_B    -   0100 0010           0x42
     * ANS_C    -   0100 0100           0x44
     * ANS_D    -   0100 1000           0x48
     * AND_E    -   1000 0001           0x81
     */
    TRISB = 0XF0;
    PORTB = 0X0F;

    /*initialize the lcd display module
        4-bit interfce
        4-lines
     */
    initialize_LCD();
    Delay_10ms();
    Delay_10ms();
    Delay_10ms();
    LCD_WriteCommand(LCD_CURSOR_OFF);
    // powerOnScreen();

    /*setup interrupts
     * TIMER 2 MODULE for genarate
     *          10ms
     */
    PR2 = 156;
    T2CON = 0b01001111; //post scaler 1:10          x10 times PR1 matches
    //pre scalar 1:16           xPR1 match every 1ms
    //module on
    PIE1bits.TMR2IE = 1; //timer 2 interrupt enable
    PIR1bits.TMR2IF = 0; //clear the timer 2 interrupt flag bit
    INTCONbits.PEIE = 1; //enable periparal interrupts
    INTCONbits.GIE = 0; //disable globle interrupts utill paper begins
    //reset timer variables
    mili_seconds = 0;
    hours = 0;
    minutes = 0;
    seconds = 0;
    //reset the display frame counter
    timeFrame_cnt = 0;



    //reset the EEPROM memory
    char addrs_cnt;
    for (addrs_cnt = EEP_PAPER_START; addrs_cnt <= 254; addrs_cnt++) {
        eeprom_write(addrs_cnt, 0);
    }


    return;
}//end of initializeDevice(void)

/**********************************
 * function configure the state of
 * the interrupt ON or OFF
 **********************************/
void interruptConfig(char state) {
    INTCONbits.GIE = state;
}

/*****************************
 * Function returns
 *  1 if user select yes
 *  0 if user select no
 * ****************************/
char get_yes_no(char lcd_location) {
    LCD_WriteStringAt(lcd_location, 10, "no");
    LCD_WriteStringAt(lcd_location + 1, 10, "yes");

    static bit user_option = 0;
    while (1) {
        LCD_WriteCharAt((char) user_option + lcd_location, 8, 0x7e); //point to the yes or no
        KEY_BUFFER = readKey();
        if (KEY_BUFFER == UP_KEY) {
            LCD_WriteCharAt((char) user_option + lcd_location, 8, ' ');
            user_option = !user_option;
            Delay_100ms();
        } else if (KEY_BUFFER == DOWN_KEY) {
            LCD_WriteCharAt((char) user_option + 2, 8, ' ');
            Delay_100ms();
            user_option = !user_option;
        } else if (KEY_BUFFER == FLAG_KEY) {
            if (user_option == 1) { //if ok to continue
                return 1;
            } else
                return 0;
        }
    }

}

/****************************
 *Function runs startup
 *screen
 ****************************/
void powerOnScreen() {
    register char counter;
    char txt0[] = "********************";
    char txt1[] = "*     Ez-examer    *";
    char txt2[] = "*    MCQ  System   *";


    LCD_WriteCommand(LCD_CLEAR);
    Delay_100ms();
    for (counter = 1; counter <= 20; counter++) {
        LCD_WriteCharAt(1, counter, 0xff); //write block caracter
        LCD_WriteCharAt(2, counter, 0xff); //write block caracter
        LCD_WriteCharAt(3, counter, 0xff); //write block caracter
        LCD_WriteCharAt(4, counter, 0xff); //write block caracter
        Delay_10ms();
        Delay_10ms();
    }

    for (counter = 20; counter > 0; counter--) {
        LCD_WriteCharAt(1, counter, 0xff); //write block caracter
        LCD_WriteCharAt(2, counter, 0xff); //write block caracter
        LCD_WriteCharAt(3, counter, 0xff); //write block caracter
        LCD_WriteCharAt(4, counter, 0xff); //write block caracter
        Delay_10ms();
        Delay_10ms();
        LCD_WriteCommand(LCD_CLEAR);
    }

    LCD_WriteStringAt(1, 1, txt0);
    LCD_WriteStringAt(2, 1, txt1);
    LCD_WriteStringAt(3, 1, txt2);
    LCD_WriteStringAt(4, 1, txt0);
    Delay_1000ms();
    Delay_1000ms();
}//End of powerOnScreen function

/**********************************
 *Fcuntion reads key from the
 *Key pad. Keys are defined top
 *of the code
 **********************************/
unsigned char readKey() {
    PORTB = 0X0F; //set default key state
    while (PORTB == 0X0F) //wait until portb changes
        ;
    unsigned char row_cntr, key;
    for (row_cntr = 1; row_cntr < 9; row_cntr <<= 1) {
        PORTB = row_cntr;
        if (PORTB == UNDO_KEY || PORTB == MENU_KEY || PORTB == UP_KEY
                || PORTB == LEFT_KEY || PORTB == RIGHT_KEY
                || PORTB == DOWN_KEY || PORTB == ANS_A_KEY || PORTB == ANS_B_KEY
                || PORTB == ANS_C_KEY || PORTB == ANS_D_KEY || PORTB == ANS_E_KEY
                || PORTB == FLAG_KEY) {

            key = PORTB;
            Delay_100ms();
            Delay_100ms();
            Delay_100ms();

            return key; //return the key
        }
    }
    return 0;
}//end of readKey(void)

/*********************************
 * Function conduct the exam
 * in normal mode
 *      -One question per page
 *      -time is displayed at top
 **********************************/
void StartExam(void) {


}

/********************************
 * Interrupt service Routing for
 * timing
 ********************************/
void interrupt INTERRUPT_SERVICE_ROUTING() {
    if (PIR1bits.TMR2IF == 1) {
        PIR1bits.TMR2IF = 0; //clear the interrupt
        mili_seconds++;
        timeFrame_cnt++;
        if (mili_seconds == 100) {
            mili_seconds = 0;
            seconds++;
            if (seconds == 60) {
                seconds = 0;
                minutes++;
                if (minutes == 60) {
                    hours = (hours + 1) % 12;
                    minutes = 0;
                }
            }
        }
    }


    if (timeFrame_cnt >= 12 && GLOBLE_TMOD == PAPER_MOD_SIMPLE) {
        LCD_WriteCharAt(1, 13, 0x30 + hours / 10);
        LCD_WriteCharAt(1, 14, 0x30 + hours % 10);
        LCD_WriteCharAt(1, 15, ':');
        LCD_WriteCharAt(1, 16, 0x30 + minutes / 10);
        LCD_WriteCharAt(1, 17, 0x30 + minutes % 10);
        LCD_WriteCharAt(1, 18, ':');
        LCD_WriteCharAt(1, 19, 0x30 + seconds / 10);
        LCD_WriteCharAt(1, 20, 0x30 + seconds % 10);
        timeFrame_cnt = 0;
    }

    //If time up
    if (hours == hourCnt && minutes == minCnt) {
        LCD_WriteCommand(LCD_CLEAR);
        LCD_WriteStringAt(2, 1, "Time up!");
        LCD_WriteStringAt(3, 1, "Hand over the device");
        LCD_WriteStringAt(4, 1, "Thank you!");
        Delay_1000ms();
        Delay_1000ms();

    }



    return;
}

