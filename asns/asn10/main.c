#include "msp.h"
#include <math.h>
#include "delay.h"

/* Objective: write a program to write a byte of data to an address
 * of the EEPROM. The data and address values can be hardcoded.
 * Then read a byte of data from same address. If the value is read correctly
 * then flash LED0. (// XXX 5ms delay in between read and write)
 * */


#define SCL BIT7
#define SDA BIT6
//=======================================================\\
// Description: init i2c
void init_I2C(uint32_t our_clk, uint32_t min_clk, uint8_t slave_addr){

	// step -1 - set gpio 
	P1->SEL1 &= ~(SCL | SDA);
	P1->SEL0 |= (SCL | SDA);

	// Step 0 - set swrst
	EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;
	// step 1 - initalize CTLW0
	EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST
		| EUSCI_B_CTLW0_MST // master mode
		| EUSCI_B_CTLW0_MODE_3 // I2C mode
		| EUSCI_B_CTLW0_SYNC // sync mode
		| EUSCI_B_CTLW0_SSEL__SMCLK; // smclk
		// | EUSCI_B0_CTLW0_ACK // ackowledge the slave address(nvr in slave tho?)
	// step 2 - BRW 
	EUSCI_B0->BRW = ceil(our_clk/min_clk) + 1;
	// step 3 - configure external slave address
	EUSCI_B0->I2CSA = slave_addr;
	// step 4 - release the swrst
	EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
	// step 5 - interrupts
}

void send_byte_EEPROM(uint8_t byte){
	// step 1 - put byte in TXBUF	
	EUSCI_B0->TXBUF = 0xFF & byte;
	// step 2 - wwait till fully sent
	while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
	// step 3 - clear ifg flag
	EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
}



//=============== Set functions for I2C=======================\\

void init_transmit(){
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR // transmitter mode
                | EUSCI_B_CTLW0_TXSTT; // generate start condition
}

void finalize_transmit(){
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP; // generate stop condition
}


/*
// Description: to tansmit a nack
void transmit_NACK(){
        EUSCI_B0->CTLW0 |= EUSCI_B0_CTLW0_TXACK; // transmit ack condition in slave mode
}

// Description: put in recieve mode
void set_asReceive(){
        EUSCI_B0->CTLW0 &= ~EUSCI_B0_CTLW0_TR; // recieve
}
*/
//========================================================\\


void transmit_to_EEPROM(uint8_t data, uint8_t device_addr, uint16_t addr_in_EEPROM){
	// step 0 - sep 
	uint8_t lower_in_EEPROM = addr_in_EEPROM & 0x00FF
		    ,higher_in_EEPROM = ((addr_in_EEPROM & 0xFF00) >> 8);
	// step 1 - I2C start condition
	init_transmit();
	// step 2 - send device addr
	send_byte_EEPROM(device_addr);
	// step 3 - -send higher addres
	send_byte_EEPROM(higher_in_EEPROM);
	// step 4 - -send lower addres
	send_byte_EEPROM(lower_in_EEPROM);
	// step 5 - send data	
	send_byte_EEPROM(data);
	// step 6 - I2C stop condition
	finalize_transmit(); 
}

/*
 * main.c
 */
void main(void)
{

	uint8_t slave_addr = 0x50 | 0x4, data = 0x69; // slave device address and data to be sent
	uint16_t addr_in_EEPROM = 0xFFFF; // address in eeprom
	uint32_t our_clk = 3*pow(10, 6), min_clk = 400 * pow(10, 3); // clk defs
	set_DCO(our_clk/pow(10, 6)); // 3 mhz
	set_clk("SMCLK");
	init_I2C(our_clk,min_clk, slave_addr);

	/*expected: 0x54, 0x5F, 0xFF, */
	while(1){
		// step 1 - send data
		transmit_to_EEPROM(data, slave_addr, addr_in_EEPROM);
		// step 2 - delay 5ms so eeprom can process it
		delay_us(5000); // delay 5ms before reading
		// step 3 - receive data
		//transmit_to_EEPROM(data, slave_addr, addr_in_EEPROM);
	}

	
}
