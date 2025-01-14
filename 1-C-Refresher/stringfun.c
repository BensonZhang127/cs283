#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
int reverse_string(buff, user_str_len);
int word_print(buff, user_str_len);


// " apple apple apple apple " -> apple apple apple apple apple...........................
// length = 23
int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    
    char *user_str_ptr = user_str; // pointer for user_str
    int user_str_len = 0;

    while (*user_str_ptr != '\0') { // count len of user_str
        user_str_len++; 
        user_str_ptr++; 
    }    

    // if counter > len, return -1
    if (user_str_len > len) { 
        return -1; 
        // -1 = The user supplied string is too large. 
        //In other words the user supplied string > BUFFER_SZ
    }

    user_str_ptr = user_str; // Reset to the beginning of the string

    // plan:
    // if character = add
    // if space ignore
    // if space before character = add space before adding character (Except for the very first word)
    char prev_char = 'z'; 
    char *buff_ptr = buff; 
    int count_buff = 0;
    int firstWordFound = 0; 

    while (*user_str_ptr != '\0') {
        if (*user_str_ptr != ' ' && *user_str_ptr != '\t') { // if not space or tab = if char then add
            if ((prev_char == ' ' || prev_char == '\t') && firstWordFound == 1) { // if space before character = add space before adding character (Except for the very first word)
                *buff_ptr = ' '; // add space
                buff_ptr++; // next index
                count_buff++; // update counter
            }

            *buff_ptr = *user_str_ptr; // add current character
            buff_ptr++; // next index
            count_buff++; // update counter
            prev_char = *user_str_ptr; // current character is now previous
            firstWordFound = 1; // update cause we found 1st word
        } else if (*user_str_ptr == ' ' || *user_str_ptr == '\t') { // if space or tab = ignore
            prev_char = ' '; // previous character is space
        }
        user_str_ptr++; 
    }


    // Fill the remaining buffer with .
    while (buff_ptr < buff + len) {
        *buff_ptr++ = '.'; 
    }

    //  If there are no errors, 
    // the setup_buff() function should return the length of the user supplied string 
    // return user_str_len;
    return count_buff; 
}


void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    char *input_string = buff; // pointer for buff
    int space_count = 0;
    int counter = 0;

    // setup_buff() already checks for len > BUFFER_SZ
    while (counter < str_len && counter < len) { // count number of spaces in buffer
        if (*input_string == ' ') { 
            space_count++; 
        }
        input_string++; 
        counter++;
    }    
    
    return space_count + 1; // return number of words (spaces + 1)
}

int reverse_string(char *buff, int str_len){
    char reversed_string[str_len]; // storage for reversed string

    // Reverse the string
    for(int rev_index=0; rev_index<str_len; rev_index++){
        *(reversed_string+rev_index) = *(buff+str_len-1-rev_index);
    }

    printf("Reversed String: %s\n", reversed_string);
    return 0;
}

int word_print(char *buff, int str_len){
    printf("Word Print\n");
    printf("----------\n");

    int row_counter = 0;    // row counter = number of words
    int each_word_starting_index = 0;    //1st word starts at index 0
    int len_counter = 0;   //length of 1st word = 0

    for (int i=0; i<=str_len; i++) { // loop through the buffer
 
        if (*(buff+i) == ' ' || i == str_len) { // if we are at a space
            // print the word
            row_counter++;
            printf("%d. ", row_counter); // 1. thing (length of thing is 5)
            
            for (int j=each_word_starting_index; j<each_word_starting_index+len_counter; j++) {
                printf("%c", *(buff+j));    
            }

            printf(" (%d)\n", len_counter);
            len_counter = 0; // reset counter
            each_word_starting_index = i+1; // next word starts at index i+1
        } else { // if we are not at a space
            len_counter++; 
            // printf("%c", *(buff+i));
        }

        
    }

    return 0;
}
            

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // We know that we are expecting something like: $ stringfun -[h|c|r|w|x] "sample string" [other_options]
    // This means that the reason why this is safe is because if argv[1] does not exist, then the the first statement (argc < 2) will not be true. 
    // argv[1] not existing is the same as doing: stringfun, which isn't valid
    // even when the argv[1] does exist, we are making a very simple check first to see if argv[1] starts with a hyphen

    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // We know that we are expecting something like: $ stringfun -[h|c|r|w|x] "sample string" [other_options]
    // The purpose for the if-statement below is to see if the user provided a 3rd argument. 
    // The 3rd argv is the string that the program will work on.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = malloc(BUFFER_SZ);
    if (buff == NULL){
        printf("Error allocating buffer\n");
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    // check length AND print 
    printf("setup_buff() returned: %d\n ", user_str_len);
    print_buff(buff,BUFFER_SZ);    
    printf("input string: %s\n", input_string);

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        case 'r':
            rc = reverse_string(buff, user_str_len);
            if (rc < 0){
                printf("Error reversing string, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;
        case 'w':
            rc = word_print(buff, user_str_len);
            if (rc < 0){
                printf("Error printing words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;
        case 'x':
            // If there are not enough arguments, print the usage and exit
            if (argc < 5) {
                printf("Not Implemented!\n");
                free(buff);
                exit(2);
                // printf("usage: %s -x \"string\" [search arg] [replace arg]\n", argv[0]);
                // free(buff);
                // exit(1);
            }

            rc = replace_string(buff, user_str_len, argv[3], argv[4]);
            if (rc < 0){
                printf("Error searching and replacing words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;            

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          There are many answers for this question, but the first thing that comes to mind 
//          is better clarity and design. Global variables can be confusing to keep track of and harder to maintain, but can allow future changes.
//          By passing the length, we allow the functions to be easier to read and understand. This also allows flexibility
//          for the function to be reused for the future if we want to handle many buffers with different sizes.
//          Also, it makes length error checking easier, we can detect if a given string is too large and prevent it messing with memory it isn't supposed to touch.
//          