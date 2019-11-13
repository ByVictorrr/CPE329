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
#define WRITE ~BIT0 // used to and with parm of write function
#define READ BIT0



// Description: init i2c:
// inputs: our_clk - master clk
//		 : min_clk - periphral clk
//		 : slave_addr - device address to communicate with
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
}


/* Description: write data to device addr at location addr_in_EEPROM*/
void transmit_to_EEPROM(uint8_t data, uint8_t device_addr, uint16_t addr_in_EEPROM){
    // step 0 - seperate values
    uint8_t lower_in_EEPROM = addr_in_EEPROM & 0x00FF
            ,higher_in_EEPROM = ((addr_in_EEPROM & 0xFF00) >> 8);
    // step 1 - I2C start condition and transmit mode
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR // transmitter mode
            | EUSCI_B_CTLW0_TXSTT; // generate start condition
    // step 2 - put device address in buff
    EUSCI_B0->TXBUF = device_addr;
	while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
	EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
	// step 3 - put higher address to write to 
	EUSCI_B0->TXBUF = higher_in_EEPROM;
	while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
	EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
	// step 4 - put lower address to write to 
	EUSCI_B0->TXBUF = lower_in_EEPROM;
	while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
	EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
	// step 5 - put the data in the buff
	EUSCI_B0->TXBUF = data;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
    EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
    // step 6 - I2C stop condition
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP; // generate stop condition
}

// Description: read data from device_addr at location addr_in_EEPROM
uint8_t read_from_EEPROM( uint8_t device_addr, uint16_t addr_in_EEPROM)
{
    uint8_t data
        ,lower_in_EEPROM = addr_in_EEPROM & 0x00FF
        ,higher_in_EEPROM = ((addr_in_EEPROM & 0xFF00) >> 8);
	// step 1 - I2C start condition and transmit mode
	EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR // transmitter mode
			| EUSCI_B_CTLW0_TXSTT; // generate start condition
	// step 2 - put device address in buff with WRITE command
	EUSCI_B0->TXBUF = device_addr & WRITE;
	while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
	EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
	// step 3 - put higher address to write to 
	EUSCI_B0->TXBUF = higher_in_EEPROM;
	while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
	EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
	// step 4 - put lower address to write to 
	EUSCI_B0->TXBUF = lower_in_EEPROM;
	while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
	EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;
	// step 5 - generate another start condition with write
	EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR; // transmitter mode
	EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // generate start condition
	// step 6 - put device address in buff with read command
	EUSCI_B0->TXBUF = device_addr | READ;
	//step 7 - send a nack then a stop
	EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXNACK; // send nack
	EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP; // generate stop condition
	// step 8 - get data
	while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0));
	data = EUSCI_B0->RXBUF;             // Read byte from the buffer
    return data;
}


/*
 * main.c
 */
void main(void)
{

	uint8_t slave_addr = 0x50//slave device address
		,tx_data = 0x69 // tx data to be send
		,rx_data = 0; // rx data to be read
	uint16_t addr_in_EEPROM = 0x7FFF; // address in eeprom
	uint32_t our_clk = 3*pow(10, 6) // msp clk
		,min_clk = 400 * pow(10, 3); // clk of periphral
	set_DCO(our_clk/pow(10, 6)); // 3 mhz
	set_clk("SMCLK");
	init_I2C(our_clk,min_clk, slave_addr);

	// To turn on led0 when tx=rx data
	P1->DIR|=BIT0;
	P1->OUT&=~BIT0;
	// clear ifg flags
	EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;
	EUSCI_B0->IFG &= ~ EUSCI_B_IFG_TXIFG0;
	// step 1 - send tx_data
	transmit_to_EEPROM(tx_data, slave_addr, addr_in_EEPROM);
	// step 2 - delay 5ms so eeprom can process it
	delay_us(5000); // delay 5ms before reading
	// step 3 - receive tx_data
	if ((rx_data = read_from_EEPROM(slave_addr, addr_in_EEPROM)) == tx_data){ 
		// if the read data is the same as the writen data
		P1->OUT|=BIT0; // turn led0 on
	}


}
