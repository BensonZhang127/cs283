#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dshlib.h"

#define DREXEL_DRAGON "dragon.txt"

/*
 * Implement your main function by building a loop that prompts the
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.  Since we want fgets to also handle
 * end of file so we can run this headless for testing we need to check
 * the return code of fgets.  I have provided an example below of how
 * to do this assuming you are storing user input inside of the cmd_buff
 * variable.
 *
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 *
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 *
 *   Also, use the constants in the dshlib.h in this code.
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *
 *   Expected output:
 *
 *      CMD_OK_HEADER      if the command parses properly. You will
 *                         follow this by the command details
 *
 *      CMD_WARN_NO_CMD    if the user entered a blank command
 *      CMD_ERR_PIPE_LIMIT if the user entered too many commands using
 *                         the pipe feature, e.g., cmd1 | cmd2 | ... |
 *
 *  See the provided test cases for output expectations.
 */



// Function to print the Drexel Dragon ASCII art from text file
void print_dragon() {
    FILE *file = fopen(DREXEL_DRAGON, "r");
    if (file == NULL) {
        printf("error: failed to open %s\n", DREXEL_DRAGON);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }
    printf("\n");

    fclose(file);
}

int main()
{

    char *cmd_buff = malloc(SH_CMD_MAX); // maximum buffer size for user input
    if (cmd_buff == NULL) {
        printf("Error allocating buffer\n");
        return -4;
    }

    
    int rc = 0;
    command_list_t clist;

    while(1){
        printf("%s", SH_PROMPT); // the shell prompt

        // get user input
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }

        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        //IMPLEMENT THE REST OF THE REQUIREMENTS     
        //  EXIT_CMD = constant that terminates the dsh program
        if(strcmp(cmd_buff, EXIT_CMD) == 0){
            exit(0); // your process should exit with a 0 exit code.
        }

        if (cmd_buff == NULL){
            // the user command was empty
            return WARN_NO_CMDS; 
        }

        if (strcmp(cmd_buff, "dragon") == 0){
            // extra credit: print dragon
            print_dragon();
            continue; 
        }

        // build command list 
        int rc = build_cmd_list(cmd_buff, &clist); 

        switch (rc){
            case OK: // 0
            //if the command parses properly. You will follow this by the command details
                printf(CMD_OK_HEADER, clist.num);
                for (int i = 0; i < clist.num; i++) {
                    printf("<%d> %s", i + 1, clist.commands[i].exe);
                    if (strlen(clist.commands[i].args) > 0) {
                        printf("[%s]", clist.commands[i].args);
                    }
                    printf("\n");
                }
                break; 
            case WARN_NO_CMDS: // -1
            // the user command was empty            
                printf(CMD_WARN_NO_CMD); 
                break; 
            case ERR_TOO_MANY_COMMANDS: // -2 
            // ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (8) commands.
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                break; 
            case ERR_CMD_OR_ARGS_TOO_BIG: // -3 
            // ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user was larger than allowed, either the executable name, or the arg string.
                fprintf(stderr, "One of the commands provided by the user was larger than allowed, either the executable name, or the arg string.\n");
                break; 
        }

    }

    exit(0);

}