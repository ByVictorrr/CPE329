#include "msp.h"
#include "LCD.h"
#include "Keypad.h"
#include <string.h>

#define MAX 1000
#define MAX_USERS 10


/* * - how to enter one of these sequences*/
#define RESET_SEQ "***"
#define CREATE_USER_SEQ "***"
#define LOGIN_SEQ "log*"
#define EXIT_SEQ "666*"



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

}


// #TODO - finish the new user fn
struct user new_user(){

}

// #TODO : memset the counter_key to '\0'

void main(void)
{

	// Var 1 - to make get sequence of char
	static char sequence_key[MAX];
	// Var 2 - ptr to the ounter_key
	static uint8_t sequence_ptr = 0
	

	while (1){	

		// ========Step 1 - check if the keypad is pushed=====
		if ((sequence_ptr[sequence_ptr] = isKeychar(4,3)) != '\0'){
			sequence_ptr++;
		}

		// ======Step 2 - go through each option=========
		// Cond 1 - see if the sequence of chars is the reset_seq
		if (strcmp(sequence_key, RESET_SEQ) == 0){
			// Reset all the counter_key
			memset(sequence_key, '\0', MAX);
		// Cond 2 - see if the sequence is to create the user
		}else if (strcmp(counter_key, CREATE_USER_SEQ) == 0){
			users[user_ptr++] = new_user();

		}else if(strcmp(counter_key, LOGIN_SEQ) == 0){
			login_user();
		}	

		delay_us(10000);

	}

}
