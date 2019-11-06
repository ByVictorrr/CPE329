/*
#define F_INPUT 3000000
#define MAX_SAMPLE 1000
#define LOW_RST 30000
#define HIGH_RST 0


float samples[MAX_SAMPLE];
float total_sqr = 0.0;

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

// For flag overflows
void TA0_N_IRQHandler(){
    ta0_ifg_overflows++;
}


// to get a new TAxR value on rising edge of wave clock
void TA0_0_IRQHandler(){
    // step 1 - clear flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    //step 2 - set the adc run
    ADC14-> CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC;
    // step 3 - ccr0 + 1/
    //TIMER_A0->CCR[0] += 50+ 1.0/(f_wave) *( F_INPUT * 0.001);
	TIMER_A0->CCR[0] += 3000;

    // step 4 - increade counter
    if(sample_count < MAX_SAMPLE){
        sample_count++;
    }else{
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
        TIMER_A0->CTL &= ~TIMER_A_CTL_IE;
    }
}


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
	TIMER_A0->CCR[0] += 3000;

    // step 3 - enable interrupts
    NVIC->ISER[0] = (1 << (TA0_N_IRQn & 0x1F)); // ifg flag
    NVIC->ISER[0] = (1 << (TA0_0_IRQn & 0x1F)); // ccr1 interupts
    TIMER_A0->R = 0;
}

bool is_valid_voltage(float high_v, float low_v){
	// cond 1 - to determine if dc value
	const float MIN_PKPK = .50;
	if (high_v >3.02 || low_v <= 0 || (high_v - low_v) < MIN_PKPK || (high_v + low_v)/2 >= 2.75){
		return FALSE;
	}
	return TRUE;
	
}


void reset_sample(float *vrms){

	// Step 1 - turn off timer ccr0
	TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
	// step 2 - get high voltage
	high_v = calibrated_voltage(high);
	// step 3 - get low voltages
	low_v = calibrated_voltage(low);	
	// setp 4 - check if the measurements are correctly
	if(is_valid_voltage(high_v, low_v)){	
		Vpp = high_v - low_v;
	}else{
		Vpp = 0;
	}
	*vrms = sqrt(total_sqr/1000);
	// Step 5 - reset all values 
	total_sqr = 0;
	sample_count = 0;
	low = LOW_RST;
	high = HIGH_RST;
	TIMER_A0 -> R = 0;
	TIMER_A0 -> CCTL[0] |= TIMER_A_CCTLN_CCIE;

}
*/
