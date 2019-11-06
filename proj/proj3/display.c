#include "msp.h"
#include "UART.h"
#include "display.h"
#include <math.h>
#include <stdio.h>

// Description: for the menu of things
void Menu(){
    UARTsendVT100(clear);
    UARTsendVT100(home);
    //UARTsendVT100(noCursor);
    sendStrUART("******************** Multimeter ********************");
    UARTsendVT100("[3;2H");   ///move cursor to line, column
    sendStrUART("DC Voltage (V) :");

    UARTsendVT100("[4;2H");   ///move cursor to line, column
    sendStrUART("Vrms (V) :");

    UARTsendVT100("[5;2H");   ///move cursor to line, column
    sendStrUART("Vpp (V) :");

    UARTsendVT100("[6;2H");   ///move cursor to line, column
    sendStrUART("Freq(Hz) :");

    UARTsendVT100("[7;2H");   ///move cursor to line, column
    sendStrUART("DC Graph:");
    UARTsendVT100("[8;2H");
    sendStrUART("(100mVperDiv)");
    UARTsendVT100("[9;2H");
    sendStrUART("RMS Graph:");
    UARTsendVT100("[10;2H");
    sendStrUART("(100mVperDiv)");

}

// Description: prints out the freq
void sendFreq (unsigned int temp){
    unsigned int split_int[]={0,0,0,0};
    unsigned int i;
        while(temp >= 1000){
                    split_int[0]+=1;
                    temp-=1000;
                }
        while(temp >= 100){
                split_int[1]+=1;
                temp-=100;
            }
            while(temp >= 10){
                split_int[2]+=1;
                temp-=10;
            }
            while(temp >= 1){
                split_int[3]+=1;
                temp-=1;
            }

            for(i = 1; i < 4; i++){
                while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
                //convert an int to char and send
                EUSCI_A0->TXBUF = (char)split_int[i] + 0x30 ;//removed 0x30
            }
            //enter cmd
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            EUSCI_A0->TXBUF = 0x0D;
            //new line cmd
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            EUSCI_A0->TXBUF = 0x0A;
            //reset array
            for(i = 0; i < 5; i++){
                split_int[i] = 0;
            }
}

// Description: makes a bar graph for a given voltage
void barGraph(float voltage){
    int bars = voltage/.1; // volts/100mv
    int i;
    for (i=0; i < bars; i++)
        sendCharUART('|');
}

//Description: prints values in their respective spots on the terminal
void getValues(float RMS, float Vpp, unsigned int freq){
   UARTsendVT100("[3;20H");   ///move cursor to line, column
   send_float_UART(RMS);


   UARTsendVT100("[4;20H");   ///move cursor to line, column
   send_float_UART(RMS);

   UARTsendVT100("[5;20H");   ///move cursor to line, column
   send_float_UART(Vpp);

   UARTsendVT100("[6;20H");   ///move cursor to line, column
   sendFreq(freq);


   UARTsendVT100("[7;15H");   ///move cursor to line, column
   barGraph(RMS);
   UARTsendVT100("[9;15H");
   barGraph(RMS);
}

