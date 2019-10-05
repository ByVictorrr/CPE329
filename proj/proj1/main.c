#include "msp.h"
#include "LCD.h"
#include "delay.h"
#include "Keypad.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define NULL 0

#define MAX_USERS 10
const signed char lookup_m[4][3] = {
            {'1','2','3'},
            {'4','5','6'},
            {'7','8', '9'},
            {'*', '0', '#'}
};



//========READ until enter char fn=========
#define ENTER_CHAR '#'
#define EXIT_CHAR '*'
#define MAX_CHARS 10
//=======================================
#define CREATE_USER (signed char)'1'
#define LOGIN (signed char)'2'

struct user{
	char username[MAX_CHARS];
	char password[MAX_CHARS];
} users[MAX_USERS];

int users_ptr = 0;
char isKeychar(int rows, int cols);
char *read_key_until_enter();
int login();
void new_user(struct user *users, int *users_ptr);
void cpy_chars(char *des, char *src, int size);

/**
 * main.c
 */



// returns: 0 if read_key() isnt in lookup
// returns: anything else 0f read_key()  is in lookup
char isKeychar(int rows, int cols){
    signed char key;
    int i, j;
    delay_us(10000);
    key = read_key();
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            if ( key == lookup_m[i][j]) // #TODO put lookup in header for Keypad.h
                return key;
        }
    }
    return '\0';
}

/*Sequence to create a new user: ***  */
/*Sequence to clear the old in key: **8   */


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
	return str;
}

void main(void)
{

	// Var 1 - to make get sequence of char
	signed char key;
	Init_LCD();
    Keypad_init();
	// Go until MAX users are created
	while (users_ptr < MAX_USERS){
	    delay_us(100000);
		Write_string_LCD("1.Create");
        delay_us(1000000);
		next_line_pos();
        delay_us(1000000);
		Write_string_LCD("2.Login");
        delay_us(1000000);


		// ========Step 1 - check to see if enter was just pressed=====
		// ======Step 2 - go through each option=========
		// Cond 1 - see if the sequence is to create the user
		if ((key=read_key()) == CREATE_USER)
			new_user(users, &users_ptr);
		// Cond 2 - see if the sequence is the login user
		else if (key == LOGIN){
            //When we're in login mode, if user_name is found then we move to password (return 0) 
            //otherwise, we keep asking for the right username. (return 1) 
			/*while(1){
                if(login() == 0)
                    break;
                }
               */
		    login();
            }
        }

		delay_us(500000);
		Clear_LCD();
		delay_us(100000);
		Home_LCD();
		//next_line_pos();
		//delay_us(100000);
		//Write_string_LCD("HI");



}


void init_user(){
    memset(users[users_ptr].password, '\0',MAX_CHARS);
    memset(users[users_ptr].username, '\0',MAX_CHARS);
}


// #TODO : login
// Description: if log in then print to lcd "unlocked"
///           : else say not unlocked an exit

// #TODO : login
// Description: if log in then print to lcd "unlocked"
///           : else say not unlocked an exit
int login(){
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
                Write_string_LCD("UNLOCKED");
                delay_us(700000);
                return 0;
            }
            free(usr_na);
            free(pss_wd);
        }
    }else{
        Clear_LCD();
        delay_us(700000);
        Write_string_LCD(usr_na);
        next_line_pos();
        Write_string_LCD("Not Found!");
        return 1;
    }
}

#define QUIT "888"
// #TODO - finish the new user fn
void new_user(struct user *users, int *users_ptr){
    char *user_key = NULL;
    char *pass_key = NULL;
    init_user();

    Clear_LCD();
    delay_us(100000);
    Write_string_LCD("Enter Username");
    delay_us(1000000);

    // Cond 1 - if the username_sequence at the base address is '\0' means user pressed enter
    if (*(user_key = read_key_until_enter()) == '\0')
        return;


    Clear_LCD();
    delay_us(100000);
    Write_string_LCD("Enter Password");
    delay_us(1000000);
    // Cond 2 - if the password_sequence at the base address is '\0' means user pressed enter
    if (*(pass_key = read_key_until_enter()) == '\0')
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


void cpy_chars(char *des, char *src, int size){
	
	int i;
	for (i=0; i < size; i++)
		des[i] = src[i];

}

// #TODO : memset the counter_key to '\0'






