/*
 * db.h
 *
 *  Created on: Oct 4, 2019
 *      Author: victor
 */

#ifndef DB_H_
#define DB_H_


#include "msp.h"
#include "LCD.h"
#include "delay.h"
#include "Keypad.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX 1000
#define MAX_USERS 10


/* * - how to enter one of these sequences*/
#define RESET_SEQ "69"
#define CREATE_USER_SEQ "99"
#define LOGIN_SEQ "11"
#define EXIT_SEQ "66"
#define QUIT "888"

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





// returns: 0 if read_key() isnt in lookup
// returns: anything else 0f read_key()  is in lookup
signed char isKeychar(int rows, int cols);

/*Sequence to create a new user: ***  */
/*Sequence to clear the old in key: **8   */


// Description: general functions that takes user input until ENTER pressed
char *read_key_until_enter();




// #TODO : login
// Description: if log in then print to lcd "unlocked"
///           : else say not unlocked an exit

// #TODO : login
// Description: if log in then print to lcd "unlocked"
///           : else say not unlocked an exit
void login();

// #TODO - finish the new user fn
void new_user(struct user *users, int *users_ptr);
#endif /* DB_H_ */
