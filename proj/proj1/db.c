#include "db.h"
#include "msp.h"
#include "LCD.h"
#include "delay.h"
#include "Keypad.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>





// returns: 0 if read_key() isnt in lookup
// returns: anything else 0f read_key()  is in lookup
char isKeychar(int rows, int cols){
    signed char key;
    int i, j;
    delay_us(10000);
    key = read_key();
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            if ( key == lookup[i][j]) // #TODO put lookup in header for Keypad.h
                return key;
        }
    }
    return '\0';
}


// Description: general functions that takes user input until ENTER pressed
char *read_key_until_enter(){
    char *str;
    int str_ptr = 0;
    next_line_pos();
    str = (char *)calloc(MAX_CHARS,sizeof(char)+1);
    // Cond 1 - keep reading till you get passed the MAX chars
    while(str_ptr < MAX_CHARS){
        // ========Step 1 - check if the keypad is pushed=====
        if ((str[str_ptr] = isKeychar(4,3)) != '\0' && str[str_ptr] != ENTER_CHAR){
            Write_char_LCD(str[str_ptr]);
            str_ptr++;
            delay_us(500000);
        }else if (str[str_ptr] == ENTER_CHAR){
            delay_us(500000);
            break;
        }
    }
	// Cond statement that makes sure the 
	if (str_ptr < 4){
		Write_string_LCD("4 Digits!");
		free(str);
		return NULL;
	}
    return str;
}


// Initalizes the user to be activated to all nulls
void init_user(){
    memset(users[users_ptr].password, '\0',MAX_CHARS);
    memset(users[users_ptr].username, '\0',MAX_CHARS);
}



// Description: After entering username, if its right it will say locked and then say unlocked after you enter the password if its right
///           : If no username is found it will say * not available.
int login(){
    char *usr_na = NULL;
    char *pss_wd = NULL;
    int i = 0;
    int found = 0;

    Clear_LCD();
    delay_us(400000);

    Write_string_LCD("Username:");
    delay_us(400000);
    next_line_pos();
    delay_us(400000);

	// less than 4 digits
	if ((usr_na = read_key_until_enter()) == NULL){
		return 1;
	}
    for (i=0 ; i < MAX_USERS ; i++){
        // see if there is user of that username in memory
        if (strcmp (usr_na, users[i].username) == 0){
            found = 1;
            break;
        }
    }

    if (found){
        while(1){
            Clear_LCD();
            delay_us(400000);
            Write_string_LCD("LOCKED");
            delay_us(400000);
            next_line_pos();
            delay_us(400000);
            Write_string_LCD("ENTER KEY");
            delay_us(1000000);
           /* while(1){
                if (isKeychar(4,3) != '\0'){
                    break;
                }
                delay_us(5000);
            }
            */
            Clear_LCD();
            delay_us(400000);


			// If less than four digits
			if ((pss_wd = read_key_until_enter()) == NULL){
				return 1;
			}


            delay_us(10000);

            if(strcmp(pss_wd, users[i].password) == 0 ){
                //psswd is the same as memory
                //print unlocked;
                free(usr_na);
                free(pss_wd);
                Clear_LCD();
                delay_us(400000);
                Write_string_LCD("UNLOCKED");
                delay_us(3000000);
                return 0;
            }
            free(usr_na);
            free(pss_wd);
        }
    }else{
        Clear_LCD();
        delay_us(400000);
        Write_string_LCD(usr_na);
        next_line_pos();
        Write_string_LCD("Not Found!");
        delay_us(3000000);
        return 1;
    }
}

// creates a new user- enter in the username and then password
void new_user(struct user *users, int *users_ptr){
    char *user_key = NULL;
    char *pass_key = NULL;
    init_user();

    Clear_LCD();
    delay_us(100000);
    Write_string_LCD("Enter Username");
    delay_us(500000);

    // Cond 1 - if the username_sequence at the base address is '\0' means user pressed enter or if its less than four digits
    if (*(user_key = read_key_until_enter()) == NULL | *user_key == '\0')
        return;


    Clear_LCD();
    delay_us(100000);
    Write_string_LCD("Enter Password");
    delay_us(500000);
    // Cond 2 - if the password_sequence at the base address is '\0' means user pressed enter or less than four digits
    if (*(pass_key = read_key_until_enter()) == NULL | *pass_key == '\0') 
        return; // #TODO or return something that indicates that the struct is empty


    char * ustruct_ptr = users[*users_ptr].username;
    char * pstruct_ptr = users[*users_ptr].password;

    cpy_chars(ustruct_ptr, user_key, strlen(user_key));
    cpy_chars(pstruct_ptr, pass_key, strlen(pass_key));

    if (user_key != NULL)
        free(user_key);
    if (pass_key != NULL)
        free(pass_key);
    *users_ptr += 1;


}


// Copys chars from src to des, with a size of size
void cpy_chars(char *des, char *src, int size){

    int i;
    for (i=0; i < size; i++)
        des[i] = src[i];

}







