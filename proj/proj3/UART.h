/*
 * UART.h
 *
 *  Created on: Oct 25, 2019
 *      Author: victor
 */

#ifndef UART_H_
#define UART_H_

#define NUMBER "0123456789"
#define MAX_CHARS 10
#define NULL 0

#define RETURN '\n'

char inValue[MAX_CHARS];




void init_UART();
uint16_t strToInt(char *str);

char intToChar(int num);
void sendCharUART(char c);


void sendStrUART(char *str);

void UARTsendVT100(char *str);


#endif /* UART_H_ */
