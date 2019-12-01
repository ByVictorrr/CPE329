#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#define NORMAL '1'
#define RESET '0'
#define NULL 0

typedef enum{NORMAL_MODE, RESTART_MODE} bt_state;
bt_state bt;
// Description: handler for hc05
void EUSCIA0_IRQHandler(void);
// Description: setup for a baud rate of 9600
void init_BT();

#endif
