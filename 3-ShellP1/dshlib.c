#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
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

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    // remove spaces from front and end
    removeSpaces(cmd_line);

    // split the command line into individual commands using '|'
    char *commands[CMD_MAX + 1]; // +1 to detect if there are too many commands
    int command_index = 0;
    char *command_token = strtok(cmd_line, PIPE_STRING);
    int len = 0;

    while (command_token != NULL) {
        // remove spaces from front and end
        removeSpaces(command_token);

        // add into commands array
        commands[command_index] = command_token;
        command_index++;

        // get next command
        command_token = strtok(NULL, PIPE_STRING);

        // if index > CMD_MAX(8), then we err
        if (command_index > CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
    }

    // if there were no commands, then err 
    if (command_index == 0) {
        return WARN_NO_CMDS;
    }

    // continue with each command (executable and arguments)
    clist->num = 0;
    for (int each_index = 0; each_index < command_index; each_index++) {
        char *exe = strtok(commands[each_index], " ");
        if (exe == NULL) {
            continue; // skip if empty commands
        }

        // check if exe >= EXE_MAX(64)
        if (strlen(exe) >= EXE_MAX) {
            // One of the commands provided by the user was larger than allowed, either the executable name, or the arg string.
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        // if valid, copy exe 
        strncpy(clist->commands[clist->num].exe, exe, EXE_MAX - 1);
        clist->commands[clist->num].exe[EXE_MAX - 1] = '\0';

        // move on to arguments
        char *arg = strtok(NULL, "");
        if (arg != NULL) {
            // remove spaces from front and end
            removeSpaces(cmd_line);

            // check if arg >= ARG_MAX(256)
            if (strlen(arg) >= ARG_MAX) {
                // One of the commands provided by the user was larger than allowed, either the executable name, or the arg string.
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }

            // if valid, copy arg 
            strncpy(clist->commands[clist->num].args, arg, ARG_MAX - 1);
            clist->commands[clist->num].args[ARG_MAX - 1] = '\0';
        } else {
            clist->commands[clist->num].args[0] = '\0'; // arg == NULL
        }

        clist->num++;
    }

    return OK;
}