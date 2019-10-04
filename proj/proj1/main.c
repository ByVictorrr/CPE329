#include "msp.h"
#include "LCD.h"
#include "Keypad.h"
#include <string.h>

#define MAX 1000
#define MAX_USERS 10


/* * - how to enter one of these sequences*/
#define RESET_SEQ "69"
#define CREATE_USER_SEQ "99"
#define LOGIN_SEQ "11"
#define EXIT_SEQ "66"


//========READ until enter char fn=========
#define ENTER_CHAR '*'
#define MAX_CHARS 10
//=======================================


struct user{
	char username[MAX];
	char password[MAX];
};

struct user users[MAX_USERS];
int users_ptr = 0;




/**
 * main.c
 */

/*Sequence to create a new user: ***  */
/*Sequence to clear the old in key: **8   */


// Description: general functions that takes user input until ENTER pressed
char *read_key_until_enter(){
	char *str;
	int str_ptr = 0;
	str = (char *)calloc(MAX_CHARS,sizeof(char)+1);
	// Cond 1 - keep reading till you get passed the MAX chars
	while(str_ptr < MAX_CHARS){
		// ========Step 1 - check if the keypad is pushed=====
		if ((str[str_ptr] = isKeychar(4,3)) != '\0' && str[str_ptr] != '*'){
			str_ptr++;
			Write_char_LCD(str[str_ptr]);
            delay_us(500000);
		}else if (str[str_ptr] == ENTER_CHAR){
            delay_us(500000);
			break;
		}
	}
	return str;
}


// returns: 0 if read_key() isnt in lookup
// returns: anything else 0f read_key()  is in lookup
signed char isKeychar(int rows, int cols){
	signed char key;
	int i, j;
	for (i = 0; i < rows; i++){
		for (j = 0; j < cols; j++){
			if ( key == lookup[i][j]) // #TODO put lookup in header for Keypad.h
				return key;
		}
	}
	return '\0';
}

// #TODO : login
// Description: if log in then print to lcd "unlocked"
///			  : else say not unlocked an exit
void login(){
	char *usr_na = NULL;
    char *pss_wd = NULL;
    int i = 0;
    int found = 0;
    
    Clear_LCD();
    delay_us(500000);
    
    Write_string_LCD("Username:");
    delay_us(1000000);
    next_line_pos();
    delay_us(1000000);
    
    usr_na = read_key_until_enter();
    for (i=0 ; i < MAX_USERS ; i++){
        // see if there is user of that username in memory
        if (strcmp (usr_na, users[i].username) == 0){
            found = 1;
            break;
        }
    }
    
    if (found){
        while(1){
            Write_string_LCD("LOCKED");
            delay_us(700000);
            next_line_pos();
            delay_us(700000);
            Write_string_LCD("Enter Key");
            delay_us(1000000);
            while(1){
                if (isKeychar(4,3) != '\0'){
                    break;
                }
                delay_us(5000);
            }
            Clear_LCD();
            delay_us(50000);
            
            pss_wd = read_key_until_enter();
            delay_us(50000);
            
            if(strcmp(pss_wd, users[i].password) == 0 ){
                //psswd is the same as memory 
                //print unlocked;
                free(usr_na);
                free(pss_wd);
                Clear_LCD();
                delay_us(700000);
                Write_string_LCD("UNCLOEDKC");
                delay_us(700000);
                return 0;
            }
            
            free(usr_na);
            free(pss_wd);
        }
    }else{
        return 1;
    }
}


#define QUIT "888"
// #TODO - finish the new user fn
void new_user(struct user *users, int *users_ptr){
	char *user_key;
	char *pass_key;
	

	Write_string_LCD("Enter Username");
	// Cond 1 - if the username_sequence at the base address is '\0' means user pressed enter
	if ((user_key = user_read_key_until_enter()) == '\0')
		return;
	}
	
	Write_string_LCD("Enter Password");
	// Cond 2 - if the password_sequence at the base address is '\0' means user pressed enter
	if ((pass_key = user_read_key_until_enter()) == '\0'){
		return; // #TODO or return something that indicates that the struct is empty
	}
	
	memcpy(users[users_ptr].username, user_key);
	memcpy(users[users_ptr].password, pass_key);
	free(user_key);
	free(pass_key);
	*users_ptr += 1;

}

// #TODO : memset the counter_key to '\0'

void main(void)
{

	// Var 1 - to make get sequence of char
	char *sequence_key;	
	Init_LCD();
    Keypad_init();
	// Go until MAX users are created
	while (users_ptr < USER_MAX){	

		Write_str_LCD("options 1- create user");
		next_line_pos();
		Write_str_LCD("option 2- login");

		// ========Step 1 - check to see if enter was just pressed=====
		sequence_key = user_read_key_until_enter();
		// ======Step 2 - go through each option=========
		// Cond 1 - see if the sequence is to create the user
		}if (strcmp(sequence_key, CREATE_USER_SEQ) == 0){
			new_user(users, &users_ptr);
		// Cond 2 - see if the sequence is the login user
		}else if(strcmp(sequence_key, LOGIN_SEQ) == 0){
			login_user();
		}		
		delay_us(10000);
		// Anything clears the sequence_key
		free(sequence_key);

	}

}
