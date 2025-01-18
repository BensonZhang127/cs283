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
int reverse_string(char *, int);
int word_print(char *, int);
int replace_target_string(char *, int, int, char *, char *);

// " apple apple apple apple " -> apple apple apple apple apple...........................
// length = 23
int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    
    char *user_str_ptr = user_str; // pointer for user_str
    int user_str_len = 0;

    // while (*user_str_ptr != '\0') { // count len of user_str
        // user_str_len++; 
        // user_str_ptr++; 
    // }    


    // user_str_ptr = user_str; // Reset to the beginning of the string

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

    // if counter > len, return -1
    if (count_buff > len) { 
        return -1; 
        // -1 = The user supplied string is too large. 
        //In other words the user supplied string > BUFFER_SZ
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
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar(']');
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
    char *reversed_string = (char *)malloc(str_len * sizeof(char)); // storage for reversed string
    if (reversed_string == NULL) {
        printf("Error allocating reversed string\n");
        return 2; //  The values that should be used are 2 = memory allocation failure
    }

    // Reverse string
    for(int rev_index=0; rev_index<str_len; rev_index++){ // index of storage start at 0
        *(reversed_string+rev_index) = *(buff+str_len-1-rev_index); // storage += last letter (negative of index)
    }    

    // copy reversed string back to buffer (for test case)
    for (int i = 0; i < str_len; i++) {
        *(buff + i) = *(reversed_string + i);
    }
    // printf("Reversed String: %s\n", reversed_string);

    free(reversed_string); // Free storage 
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
            printf("%d. ", row_counter); // 1. thing (5)
            
            for (int j=each_word_starting_index; j<each_word_starting_index+len_counter; j++) { // basically substring(index, index+counter)
                printf("%c", *(buff+j));    
            }

            printf("(%d)\n", len_counter);
            len_counter = 0; // reset counter
            each_word_starting_index = i+1; // next word starts at index i+1

        } else { // if we are not at a space
            len_counter++; 
            // printf("%c", *(buff+i));
        }

        
    }

    printf("\nNumber of words returned: %d\n", row_counter);

    return 0;
}
            
int replace_target_string(char *buff, int len, int str_len, char *target, char *replace){
    int target_len = 0; // length of target is 0
    int replace_len = 0; // length of replacement string is 0

    // Get the true length of target string and replacement string
    while (*(target+target_len) != '\0'){
        target_len++;
    }
    while (*(replace+replace_len) != '\0'){
        replace_len++;
    } 

    // // if after replacing the string, it exceeds buffer_size (len), then error
    // if (str_len - target_len + replace_len > len){
    //     printf("Error: new replacement string exceeds buffer size\n");
    //     return -1;
    // }

    // if target/replace length is longer than buffer_size, then error
    if (target_len > len) { 
        printf("Error: searching string should not exceed buffer size\n");
        return -2;
    }
    if (replace_len > len){
        printf("Error: replacement string should not exceed buffer size\n");
        return -2;
    }

    // i think i should test if string is empty, in-case it bypass argv checks
    // so if target/replace length is 0, then error
    if (target_len == 0) { 
        printf("Error: searching string should not be 0\n");
        return -3;
    }
    if (replace_len == 0){
        printf("Error: replacement string should not be 0\n");
        return -3;
    }

    // target should be <= input_string length
    if (!(target_len <= str_len)){
        printf("Error: target string's length should be at max input string's length\n");
        return -4;
    }



    // stringfun -x     "Replacing words in strings is not fun!" not super
    // not -> super 
    // Modified String: "Replacing words in strings is super fun!"

    char *new_string = (char *)malloc((str_len+replace_len-target_len+1) * sizeof(char)); // storage for reversed string
    if (new_string == NULL) {
        printf("Error allocating new string\n");
        return -5;
    }

    char *buffer_string = buff; // pointer for buffer
    int target_starting_index = 0;    //assume target is at index 0
    int assumed_found = 1; // assume target found
    int target_found = 0; // target initially has not been found

    // Assume target found
    // If after looping through target_length, and we don't found a mismatch, then we know it is actually found.
    // If mismatch found = go next index
    // If mismatch not found = target is found and we need to replace the string

    for (int i = 0; i <= str_len-replace_len; i++) {
        assumed_found = 1; // assume target found 

        for (int target_loop = 0; target_loop<target_len; target_loop++){
            if (*(buffer_string+i+target_loop) != *(target+target_loop)) { // if characters dont match
                assumed_found = 0;
                break; 
            } // go next index
        }

        if (assumed_found == 1){
            target_starting_index = i; 
            target_found = 1;
            break;
        }
    }
    
    // for the else ++ and copy loop after 
    int index = 0;

    if (target_found == 0){
        printf("Error: Target string was not found\n");
        free(new_string);
        return -6;
    } else {

        // Else if it was found we need to replace the target string
        // Add Beginning part
        // Add replace
        // Add Rest of string


        // copy the beginning
        for (int i = 0; i < target_starting_index; i++) {
            *(new_string + index) = *(buff + i);
            index++;
        }

        // copy the replacement 
        for (int i = 0; i < replace_len; i++) {
            *(new_string + index) = *(replace + i);            
            index++;
        }

        // copy the rest
        for (int i = target_starting_index + target_len; i < str_len; i++) {
            *(new_string + index) = *(buff + i);            
            index++;
        }

        // terminate to be able to print
        *(new_string + index) = '\0';

    } 


    // remove buffer
    for (int i = 0; i < str_len; i++) {
        *(buff + i) = '.';
    }

    // copy modified string back to buffer (for test case)
    for (int i = 0; i < index; i++) {
        *(buff + i) = *(new_string + i);
    }
    // printf("Modified String: ");
    // printf("%s\n", new_string);

    free(new_string);
    
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
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }


    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
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
                // printf("Not Implemented!\n");
                printf("Not enough parameters\n");
                free(buff);
                exit(1); // 1 = command line problem
            }
            
            
            
            // stringfun -x     "Replacing words in strings is not fun!" not super
            // not -> super 
            // Modified String: "Replacing words in strings is super fun!"
            rc = replace_target_string(buff, BUFFER_SZ, user_str_len, argv[3], argv[4]);
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
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
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