#include "msp.h"
#include "UART.h"
#include "ADC.h"
#include "delay.h"
#include <math.h>
#include "UART.h"
#include <stdio.h>


//===============================frequence stuff (freq stuff) ==================================\\

float f_wave = 0;
int freq_flag = 0;
int overflow_ta0 = 0;
int ta0_ifg_overflows = 0;

#define F_INPUT 3000000

uint32_t st_r = 0.0;
uint32_t en_r = 0.0;
uint32_t hi_lo = 1;


void init_TA1(){
    //Configure Timer 0 for interrupt
        TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, continuous mode
                TIMER_A_CTL_MC__CONTINUOUS|
                TIMER_A_CTL_IE; // enable TAxR overflow

        TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CM_1 | //Trigger on rising edge
                TIMER_A_CCTLN_CCIS_0 | //select CCIxA
                TIMER_A_CCTLN_CCIE | //enable interrupt
                TIMER_A_CCTLN_CAP | // capture mode
                TIMER_A_CCTLN_SCS; //synchronous

        TIMER_A1->CTL |= TIMER_A_CTL_TASSEL_2 | //SMCLK
                TIMER_A_CTL_MC__CONTINUOUS;

        NVIC->ISER[0] = 1 << ((TA1_N_IRQn) & 31);
        NVIC->ISER[0] = 1 << ((TA1_0_IRQn) & 31);
        __enable_irq();
}
// Description: for flag overflows
void TA1_N_IRQHandler(){
    overflow_ta0++;
    TIMER_A1->CTL &= ~TIMER_A_CTL_IFG; // clear interrupt flag
}
uint32_t getRealFreq(float wavefreq){
    uint32_t real_f = 0;

    // real ranges [0,46]
    if (wavefreq > 0 && wavefreq < 9.37){
        real_f = 2.09*wavefreq - .107;
    }else if(wavefreq> 9.37 && wavefreq <= 16){
        real_f = 3.49*wavefreq-14.4;
    }else if(wavefreq >16 && wavefreq <=99 ){
        real_f = (int)wavefreq;
    }else if (wavefreq>99 && wavefreq <= 170 ){
        real_f = ceil(wavefreq);
    }else if (wavefreq>170 && wavefreq <= 300){
        real_f = ceil(wavefreq) + 1;
    }else if (wavefreq>300 && wavefreq <= 430){
        real_f = ceil(wavefreq) + 2;
    }else if (wavefreq>430 && wavefreq <= 590){
        real_f = ceil(wavefreq) + 3;
    }else if (wavefreq>590 && wavefreq <=700){
        real_f = ceil(wavefreq) + 4;
    }else if (wavefreq >700 && wavefreq <=830){
        real_f = ceil(wavefreq) + 5;
    }else if (wavefreq >830 && wavefreq <=1000){
        real_f = ceil(wavefreq) + 6;
    }

    return real_f;
}
// Description: checks if over overflow if so 
void TA1_0_IRQHandler(){

      if (hi_lo == 1){//not first point
          st_r = (0xffff)*overflow_ta0 + TIMER_A1->CCR[0];

          hi_lo =0;
      }else { //second point
          if (TIMER_A1->CCTL[0] & COV){
              TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_COV;
              overflow_ta0 = 0;
              TIMER_A1->CTL |= TIMER_A_CTL_CLR;
              hi_lo = 1;
          }else{
            en_r = (0xffff)*overflow_ta0 + TIMER_A1->CCR[0];
            f_wave =   F_INPUT/((en_r - st_r));
            f_wave = getRealFreq(f_wave);
            hi_lo = 1;
            overflow_ta0 = 0;
            TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
            freq_flag = 1;
            if (f_wave > 1000){
                hi_lo =1;
            }

          }
      }

      TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}
void disable_TA1(){
    TIMER_A1->CTL &= ~TIMER_A_CTL_IE; // disable interputs for TAxR overflowing
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIE; // disable interupts
}

void get_freq(){
    init_TA1();
    while(freq_flag == 0);
    disable_TA1();
}


//================================================================================================\\





//===============================Sample stuff (vpp, vrms stuff) ==================================\\

#define MAX_SAMPLE 1000
#define LOW_RST 30000
#define HIGH_RST 0


float samples[MAX_SAMPLE];
float total_sqr = 0.0;
float Vppp;
float Vrmss;

typedef enum{FALSE, TRUE} bool;

int low, high; // for high and low values
int sample_count = 0;


// Description: dont add more to tis shit
void ADC14_IRQHandler(){
    float voltage;
    if (high < (digital = ADC14->MEM[22])){
        high = digital;
    }
    if (low > digital){
        low = digital;
    }
    voltage = calibrated_voltage(digital);
    total_sqr = total_sqr + voltage*voltage;
}

// to get a new TAxR value on rising edge of wave clock
void TA0_0_IRQHandler(){
    // step 1 - clear flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    //step 2 - set the adc run
    ADC14-> CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC;
    // step 3 - ccr0 + 1/
    TIMER_A0->CCR[0] += 3000;

    // step 4 - increade counter
    if(sample_count < MAX_SAMPLE){
        sample_count++;
    }else{
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
        TIMER_A0->CTL &= ~TIMER_A_CTL_IE;
    }
}

//void disable_TA0(){
//    TIMER_A0->CTL &= ~ TIMER_A_CTL_IE; // enable interputs for TAxR overflowing
//    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE; // enable interupts
//}
//void enable_TA0(){
//  //  TIMER_A0->CTL &= ~ TIMER_A_CTL_IE; // enable interputs for TAxR overflowing
//       TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interupts
//}
void init_TA0(){
    // step 1 - configure ctl reg
    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 // select smclk
                    | TIMER_A_CTL_MC_2 // up mode
                    | TIMER_A_CTL_IE; // enable interputs for TAxR overflowing
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; // clear interrupt flag
    // step 2 - config cctl[0]
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CM_1 // capture on rising edge
                                             // use CCI0A
                        | TIMER_A_CCTLN_SCS // TODO: not sure if enable synchrounous
                        | TIMER_A_CCTLN_CCIE; // enable interupts
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

    // step 4 - initalize ccr0
    //TIMER_A0->CCR[0] = 50 + (1.0/(f_wave)* 3000000.0 * 0.001);
	TIMER_A0->CCR[0] = 3000;

    // step 3 - enable interrupts
    NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // ccr1 interupts
    TIMER_A0->R = 0;
}



void get_vpp_vrms(float *Vrms, float *Vpp){
    init_TA0();
    float high_v, low_v;
    low = 30000;
    high = 0;
    sample_count = 0;
    while(1){
        if(sample_count >= 1000){
            TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
            high_v = calibrated_voltage(high);
            low_v = calibrated_voltage(low);


            // cond 1 - to determine if dc value
            if (low_v <= 0){
                high_v = 0;
                low_v = 0;
            }else if ((Vppp = high_v - low_v) <0.49){
                high_v = low_v = 0;
            }else if ((high_v + low_v)/2 >= 2.75){
                high_v = 0;
                low_v = 0;
            }

            *Vpp = Vppp = high_v - low_v;

            *Vrms = Vrmss = sqrt(total_sqr/1000);
            total_sqr = 0;
            sample_count = 0;
            low = 30000;
            high = 0;

            TIMER_A0 -> R = 0;
            break;
        }
    }

}


//===================================================================\\

//==========================menu shit=======================================\\

#define clear "[2J"       //VT100 cmd to clear whole screen
#define home "[H"         //VT100 cmd to home the cursor home
#define clearLine "[2k"        //Clears the whole line
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
    sendStrUART("(10mVperDiv)");
    UARTsendVT100("[9;2H");
    sendStrUART("RMS Graph:");
    UARTsendVT100("[10;2H");
    sendStrUART("(100mVperDiv)");

}
// converts voltage value into UART signals
void sendVoltage (unsigned int temp){
    unsigned int split_int[]={0,0,0,0};
    unsigned int i;
        while(temp >= 10000){
                    split_int[0]+=1;
                    temp-=10000;
                }
        while(temp >= 1000){
                split_int[1]+=1;
                temp-=1000;
            }
            while(temp >= 100){
                split_int[2]+=1;
                temp-=100;
            }
            while(temp >= 10){
                split_int[3]+=1;
                temp-=10;
            }
    // send 1s place
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
                                  EUSCI_A0->TXBUF = (char)split_int[0] + 0x30;
                                  //send decimal point
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
                                  EUSCI_A0->TXBUF = 0x2E;
//send rest of the number
            for(i = 1; i < 4; i++){
                while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
                //convert an int to char and send
                EUSCI_A0->TXBUF = (char)split_int[i]+0x30 ;//removed 0x30
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
//__________________________________________________________________________________________________

// same as sendVoltage
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
void barGraph(float voltage){
    int bars = voltage/.1; // volts/100mv
    int i;
    for (i=0; i < bars; i++)
        sendCharUART('|');
}

//prints values in their respective spots on the terminal
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





//===================================================================\\

void main(void){

    P8->SEL0&=~BIT0;
    P8->SEL1|=BIT0;
    P8->DIR&=~BIT0;
    set_clk("SMCLK");
    set_DCO(F_INPUT/1000000);
    init_UART();
    init_ADC14();
    __enable_irq();
     //step 1 - configure ctl reg
    float Vrms, Vpp;

    low = LOW_RST;  //Low equavilent of 3V
    high = HIGH_RST;  //High equivalent o 0v
    sample_count = 0;


    while(1){
        get_freq();
        freq_flag=0;
//        get_vpp_vrms(&Vrms, &Vpp);
        // print out bar graph
        Menu();
        getValues(Vrms,Vpp,f_wave);
    }


}
