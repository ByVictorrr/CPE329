#include "msp.h"
#include "LCD.h"
#include "Keypad.h"
#include "db.h"
#include "delay.h"



void main(void)
{

    // Var 1 - to make get sequence of char
    signed char key;
    Init_LCD();
    Keypad_init();
    // Go until MAX users are created
    while (users_ptr < MAX_USERS){
        Write_string_LCD("1.Create");
        delay_us(350000);
        next_line_pos();
        delay_us(350000);
        Write_string_LCD("2.Login");
        delay_us(350000);

        // ========Step 1 - check to see if enter was just pressed=====
        // ======Step 2 - go through each option=========
        // Cond 1 - see if the sequence is to create the user
        while (1){
            if ((key=read_key()) == CREATE_USER){
                new_user(users, &users_ptr);
                break;
            }
            // Cond 2 - see if the sequence is the login user
            else if (key == LOGIN){
                //When we're in login mode, if user_name is found then we move to password (return 0)
                //otherwise, we keep asking for the right username. (return 1)
                login();
                break;
                }
        }
        delay_us(350000);
        Clear_LCD();
        delay_us(350000);

    }
}




