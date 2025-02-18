#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
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
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
*/


void removeSpaces(char *string)
{
    // remove spaces from beginning (left)
    char *begin = string;
    while (isspace(*begin))
    {
        begin++;
    }

    // remove spaces from end (right)
    // last position = starting position + length - 1
    char *end = string + strlen(string) - 1;
    while (end > begin && isspace(*end))
    {
        end--;
    }
    *(end + 1) = '\0';
}
 

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    // alloc max buffer size for user input
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX * sizeof(char)); 
    if (cmd_buff->_cmd_buffer == NULL) {
        printf("Error allocating buffer\n");
        return ERR_MEMORY;
    }

    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX); 

    cmd_buff->argc = 0;

    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv)); 

    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff->_cmd_buffer != NULL) { 
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL; 
    }
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->argc = 0;
    cmd_buff->_cmd_buffer[0] = '\0';
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    removeSpaces(cmd_line);
    strcpy(cmd_buff->_cmd_buffer, cmd_line);
    return OK;
}

Built_In_Cmds match_command(const char *input)
{
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else {
        return BI_NOT_BI;
    }
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    if (strcmp(cmd->argv[0], "exit") == 0) {
        return BI_EXECUTED;
    } else if (strcmp(cmd->argv[0], "dragon") == 0) {
        return BI_EXECUTED;
    } else if (strcmp(cmd->argv[0], "cd") == 0) {
        return BI_EXECUTED;
    } else {
        return BI_RC;
    }
}

int exec_cmd(cmd_buff_t *cmd)
{
    return OK;
}

 int exec_local_cmd_loop()
{
    char *cmd_buff;
    int rc = OK;
    cmd_buff_t cmd;

    while(1){
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }
        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
     
        //IMPLEMENT THE REST OF THE REQUIREMENTS
    }


    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    return OK;
}