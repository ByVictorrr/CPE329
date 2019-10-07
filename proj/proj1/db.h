#ifndef DB_H_
#define DB_H_
#define NULL 0
#define ENTER_CHAR '#'
#define EXIT_CHAR '*'
#define MAX_CHARS 10
#define MAX_USERS 10
#define CREATE_USER (signed char)'1'
#define LOGIN (signed char)'2'

// Struct of users with usernames and passwords
static struct user{
    char username[MAX_CHARS];
    char password[MAX_CHARS];
} users[MAX_USERS];
static int users_ptr = 0;

char isKeychar(int rows, int cols); //Return the key pressed on keypad
char *read_key_until_enter();//Read keys until "#" is pressed
int login();//Login interface, enter the correct username password combination to show UNLOCKED screen
void new_user(struct user *users, int *users_ptr);// Create a new user/password combination to put in db
void cpy_chars(char *des, char *src, int size);
void init_user(); // initializes a new users password and username
void cpy_chars(char *des, char *src, int size); // copys whatever is in src to des, size says how many element want to copy



#endif
