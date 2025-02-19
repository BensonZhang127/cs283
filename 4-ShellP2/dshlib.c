#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

#include <errno.h>

extern void print_dragon(); // dragon.c

int last_rc = 0;

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

    if (cmd_buff->_cmd_buffer) { 
        memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX); 
    }

    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv)); 
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    // set up
    clear_cmd_buff(cmd_buff); 

    removeSpaces(cmd_line);
    
    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1); 
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';


    char *token = cmd_buff->_cmd_buffer; // Use the buffer directly
    int argc = 0;
    bool in_quotes_mode = false;
    char *start = NULL; 

    while (*token) {

        if (isspace((char)*token)){ // if space
            if (in_quotes_mode == false){
                token++;
                continue;
            }
        }


        if (*token == '\"') { // if quote 
            in_quotes_mode = !in_quotes_mode;
            if (in_quotes_mode) {
                start = token + 1;
            } else {
                *token = '\0';
                if (argc < CMD_MAX) {
                    cmd_buff->argv[argc++] = start;
                }
            }
            token++;
            continue;
        }

        start = token;
        if (in_quotes_mode) { // true -> in quote mode
            while (*token && *token != '\"') { 
                token++;
            }
            if (*token == '\"') {
                *token = '\0'; 
                in_quotes_mode = false; 
                token++; 
            }
        } else { // false -> in quote mode
            while (*token && !isspace((char)*token)) { 
                if (*token == '\"') {
                    break; 
                }
                token++;
            }
            if (*token) {
                *token = '\0'; 
                token++; 
            }
        }
        if (argc < CMD_MAX) {
            cmd_buff->argv[argc++] = start; // Store the argument
        }


    } // end while 

    cmd_buff->argc = argc;
    cmd_buff->argv[argc] = NULL;

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
    } else if (strcmp(input, "rc") == 0) {
        return BI_RC;
    } else {
        return BI_NOT_BI;
    }
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    int rc = BI_NOT_BI;
    
    if (cmd_type == BI_CMD_EXIT) {
        rc = BI_CMD_EXIT;
    } else if (cmd_type == BI_CMD_DRAGON) {
        print_dragon();
        rc = BI_EXECUTED;
    } else if (cmd_type == BI_CMD_CD) {
        if (cmd->argc == 1) {
            // nothing happens
            rc = BI_EXECUTED;
        } else if (cmd->argc == 2) { 
            if (chdir(cmd->argv[1]) != 0) {
                printf("error: could not change directory to %s\n", cmd->argv[1]); 
            }
            rc = BI_EXECUTED;
        } else { 
            printf("error: too many arguments for cd\n"); 
            rc = BI_EXECUTED;
        }
    } else if (cmd_type == BI_RC) {
        printf("%d\n", last_rc); 
        rc = BI_EXECUTED;
    } else {
        rc = BI_NOT_BI;
    }

    return rc;
}

int exec_cmd(cmd_buff_t *cmd)
{
    int f_result, c_result;
    
    f_result = fork();
    if (f_result < 0) {
        perror("fork error");
        return ERR_EXEC_CMD; 
    }
    
    if (f_result == 0) {
        // The child will now exec, basically shape shifting itself
        int rc = execvp(cmd->argv[0], cmd->argv);
        if (rc < 0){
            perror("fork error"); 
            exit(errno);     
        }
    } else {
        // This will be where the parent picks up
        waitpid(f_result, &c_result, 0);

        if (WIFEXITED(c_result)) {
            int rc = WEXITSTATUS(c_result);

            if (rc==0){
                return OK;
            } else if (rc==ENOENT) {
                printf("Error: file not found\n");
                return rc;
            } else if (rc==EACCES) {
                printf("Error: permission denied\n");
                return rc;
            } else if (rc==ENOEXEC) {
                printf("Error: executable format error\n");
                return rc;
            } else if (rc==EIO) {
                printf("Error: input/output error\n");
                return rc;
            } else {
                printf("Command execution failed with code %d\n", rc);
                return rc;
            }
        } else {
            printf(CMD_ERR_EXECUTE);
            return ERR_EXEC_CMD; 
        }
    }

    return OK;
}

 int exec_local_cmd_loop()
{
    char *cmd_input = malloc(SH_CMD_MAX * sizeof(char));
    int rc = OK;
    cmd_buff_t cmd_buff;

    // Allocate memory for cmd_buff
    rc = alloc_cmd_buff(&cmd_buff);
    if (rc == ERR_MEMORY) {
        printf("Error allocating buffer\n");
        return rc;
    }

    while(1){
        printf("%s", SH_PROMPT);
        if (fgets(cmd_input, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }
        //remove the trailing \n from cmd_buff
        cmd_input[strcspn(cmd_input,"\n")] = '\0';

        rc = build_cmd_buff(cmd_input, &cmd_buff);

        if (cmd_buff.argc == 0){
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        rc = exec_built_in_cmd(&cmd_buff);

        if(rc == BI_CMD_EXIT){
            rc = OK_EXIT;
            break;
        }
        if(rc == BI_EXECUTED){
            rc = OK;
            continue;
        }
        
        rc = exec_cmd(&cmd_buff);

        last_rc = rc;

     
    }

    free(cmd_input);
    free_cmd_buff(&cmd_buff);

    return rc;
}