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
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_buff->_cmd_buffer == NULL) {
        printf("Error allocating buffer\n");
        return ERR_MEMORY;
    }

    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);

    cmd_buff->argc = 0;

    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));

    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    cmd_buff->outputAppendMode = 0;

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

    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    cmd_buff->outputAppendMode = 0;
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    // set up
    clear_cmd_buff(cmd_buff);

    removeSpaces(cmd_line);

    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';


    char *token = cmd_buff->_cmd_buffer; 
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

    int outputArgumentCount = 0;
    for (int currentArgumentIndex = 0; currentArgumentIndex < argc; currentArgumentIndex++) {
        if (strcmp(cmd_buff->argv[currentArgumentIndex], "<") == 0) {
            if (currentArgumentIndex == 0) { // Check if '<' is at the beginning
                fprintf(stderr, "Error: Missing command before input redirection.\n");
                return -4; // Return error code
            }
            if (currentArgumentIndex + 1 < argc) {
                cmd_buff->input_file = cmd_buff->argv[currentArgumentIndex + 1];
                currentArgumentIndex++;
            } else {
                fprintf(stderr, "Error: Missing filename after input redirection.\n");
                return -4;
            }
        } else if (strcmp(cmd_buff->argv[currentArgumentIndex], ">") == 0) {
            if (currentArgumentIndex == 0) { // Check if '>' is at the beginning
                fprintf(stderr, "Error: Missing command before output redirection.\n");
                return -4; // Return error code
            }
            if (currentArgumentIndex + 1 < argc) {
                cmd_buff->output_file = cmd_buff->argv[currentArgumentIndex + 1];
                cmd_buff->outputAppendMode = 0;
                currentArgumentIndex++;
            } else {
                fprintf(stderr, "Error: Missing filename after output redirection.\n");
                return -4;
            }
        } else if (strcmp(cmd_buff->argv[currentArgumentIndex], ">>") == 0) {
            if (currentArgumentIndex == 0) { // Check if '>>' is at the beginning
                fprintf(stderr, "Error: Missing command before append output redirection.\n");
                return -4; // Return error code
            }
            if (currentArgumentIndex + 1 < argc) {
                cmd_buff->output_file = cmd_buff->argv[currentArgumentIndex + 1];
                cmd_buff->outputAppendMode = 1;
                currentArgumentIndex++;
            } else {
                fprintf(stderr, "Error: Missing filename after append output redirection.\n");
                return -4;
            }
        } else {
            cmd_buff->argv[outputArgumentCount++] = cmd_buff->argv[currentArgumentIndex];
        }
    }
    
    cmd_buff->argv[outputArgumentCount] = NULL;
    cmd_buff->argc = outputArgumentCount;



    return OK;
}


int build_cmd_list(char *cmd_line, command_list_t *clist) {
    removeSpaces(cmd_line);
    clist->num = 0; 

    char *pointer_check_pipe = cmd_line;
    while (*pointer_check_pipe == ' ' || *pointer_check_pipe == '\t') {
        pointer_check_pipe++;
    }
    if (*pointer_check_pipe == '|' || (strlen(pointer_check_pipe) > 0 && pointer_check_pipe[strlen(pointer_check_pipe) - 1] == '|')) {
        return ERR_EXEC_CMD;  
    }

    char *command_token = strtok(cmd_line, PIPE_STRING);
    int command_index = 0;
    int last_was_empty = 0; 

    while (command_token != NULL) {
        removeSpaces(command_token);

        if (command_index >= CMD_MAX) {
            free_cmd_list(clist);  
            return ERR_TOO_MANY_COMMANDS;
        }

        if (strlen(command_token) == 0) {
            if (last_was_empty || command_index == 0) {
                free_cmd_list(clist);
                return ERR_TOO_MANY_COMMANDS;
            }
            last_was_empty = 1;
            command_token = strtok(NULL, PIPE_STRING);
            command_index++;
            continue;
        }

        last_was_empty = 0;

        cmd_buff_t current_cmd;
        int alloc_result = alloc_cmd_buff(&current_cmd);
        if (alloc_result != OK) {
            free_cmd_list(clist);
            return alloc_result;
        }

        int result = build_cmd_buff(command_token, &current_cmd);
        if (result != OK) {
            free_cmd_buff(&current_cmd);
            free_cmd_list(clist);
            return result;
        }

        clist->commands[clist->num] = current_cmd;
        clist->num++;

        command_token = strtok(NULL, PIPE_STRING);
        command_index++;
    }

    if (last_was_empty) {
        free_cmd_list(clist);
        return ERR_TOO_MANY_COMMANDS;
    }

    if (clist->num == 0) {
        fprintf(stderr, CMD_WARN_NO_CMD);
        return WARN_NO_CMDS;
    }

    return OK;
}


int free_cmd_list(command_list_t *cmd_list) {
    for (int i = 0; i < cmd_list->num; i++) {
        free_cmd_buff(&cmd_list->commands[i]);
    }
    cmd_list->num = 0;
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

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]); // Determine the built-in command
    int rc = BI_NOT_BI; // Initialize return code to "not a built-in command"

    if (cmd_type == BI_CMD_EXIT) {
        // Handle the 'exit' command directly
        rc = BI_CMD_EXIT; // Indicate exit command was found
    } else if (cmd_type == BI_CMD_DRAGON) {
        // Handle the 'dragon' command
        handle_redirection(cmd); // Apply any redirection specified in the command
        print_dragon(); // Execute the dragon drawing function
        rc = BI_EXECUTED; // Indicate successful execution
    } else if (cmd_type == BI_CMD_CD) {
        // Handle the 'cd' command
        if (cmd->argc == 1) {
            // If no directory is specified, do nothing (or change to HOME, if desired)
            rc = BI_EXECUTED;
        } else if (cmd->argc == 2) {
            // Change directory to the specified argument
            if (chdir(cmd->argv[1]) != 0) {
                // If chdir fails, print an error message
                printf("error: could not change directory to %s\n", cmd->argv[1]);
            }
            rc = BI_EXECUTED;
        } else {
            // If too many arguments are provided, print an error message
            printf("error: too many arguments for cd\n");
            rc = BI_EXECUTED;
        }
    } else if (cmd_type == BI_RC) {
        // Handle the 'rc' command (print the last return code)
        handle_redirection(cmd); // Apply any redirection specified in the command
        printf("%d\n", last_rc); // Print the last return code
        rc = BI_EXECUTED; // Indicate successful execution
    } else {
        // If the command is not a recognized built-in, leave rc as BI_NOT_BI
        rc = BI_NOT_BI;
    }

    return rc; // Return the result
}


void handle_redirection(cmd_buff_t *cmd) {
    if (cmd->input_file!=NULL) {
        int fd_in = open(cmd->input_file, O_RDONLY);
        if (fd_in < 0) {
            perror("open input");
            exit(errno);
        }
        if (dup2(fd_in, STDIN_FILENO) < 0) {
            perror("dup2 input");
            exit(errno);
        }
        close(fd_in);
    }
    if (cmd->output_file!=NULL) {
        int fd_out;
        if (cmd->outputAppendMode)
            fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND , 0644);
        else
            fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC , 0644);
        if (fd_out < 0) {
            perror("open output");
            exit(errno);
        }
        if (dup2(fd_out, STDOUT_FILENO) < 0) {
            perror("dup2 output");
            exit(errno);
        }
        close(fd_out);
    }
}

int exec_cmd(cmd_buff_t *cmd)
{
    int f_result, c_result;

    f_result = fork();
    if (f_result < 0) {
        perror(CMD_ERR_EXECUTE);
        return ERR_EXEC_CMD;
    } else if (f_result == 0) {
        // The child will now exec, basically shape shifting itself

        handle_redirection(cmd);

        int rc = execvp(cmd->argv[0], cmd->argv);
        if (rc < 0){
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
                printf("error: file/directory/command not found\n");
                return rc;
            } else if (rc==EACCES) {
                printf("error: permission denied\n");
                return rc;
            } else {
                printf("Failed to execute command with rc %d\n", rc);
                return rc;
            }
        } else {
            printf(CMD_ERR_EXECUTE);
            return ERR_EXEC_CMD;
        }
    }

    return OK;
}


int exec_pipeline(command_list_t *command_list) {
    int num_commands = command_list->num;
    if (num_commands <= 0) {
        return WARN_NO_CMDS;  
    }

    pid_t child_pids[CMD_MAX];
    int previous_pipe_read_fd = -1;
    int last_command_status = OK;

    for (int command_index = 0; command_index < num_commands; command_index++) {
        if (!command_list->commands[command_index].argv[0] || strlen(command_list->commands[command_index].argv[0]) == 0) {
            fprintf(stderr, "error: invalid command in pipeline\n");
            return ERR_EXEC_CMD;
        }

        int current_pipe_fds[2] = {-1, -1};
        if (command_index < num_commands - 1) {
            if (pipe(current_pipe_fds) < 0) {
                perror("pipe creation failed");
                return ERR_MEMORY;
            }
        }

        child_pids[command_index] = fork();
        if (child_pids[command_index] < 0) {
            perror("fork failed");
            return ERR_EXEC_CMD;
        }

        if (child_pids[command_index] == 0) {
            if (previous_pipe_read_fd != -1) {
                if (dup2(previous_pipe_read_fd, STDIN_FILENO) < 0) {
                    perror("dup2 Error redirecting standard input");
                    exit(errno);
                }
                close(previous_pipe_read_fd);
            }

            if (command_index < num_commands - 1) {
                if (dup2(current_pipe_fds[1], STDOUT_FILENO) < 0) {
                    perror("dup2 Error redirecting standard output");
                    exit(errno);
                }
                close(current_pipe_fds[0]);
                close(current_pipe_fds[1]);
            }

            handle_redirection(&command_list->commands[command_index]);

            if (match_command(command_list->commands[command_index].argv[0]) != BI_NOT_BI) {
                exec_built_in_cmd(&command_list->commands[command_index]);
                exit(0);
            } else {
                if (execvp(command_list->commands[command_index].argv[0], command_list->commands[command_index].argv) < 0) {
                    perror("execvp");
                    exit(errno);
                }
                exit(0);
            }
        } else {
            if (previous_pipe_read_fd != -1) {
                close(previous_pipe_read_fd);
            }
            if (command_index < num_commands - 1) {
                previous_pipe_read_fd = current_pipe_fds[0];
                close(current_pipe_fds[1]);
            }
        }
    }

    for (int command_index = 0; command_index < num_commands; command_index++) {
        int child_status;
        waitpid(child_pids[command_index], &child_status, 0);

        if (WIFEXITED(child_status)) {
            int rc = WEXITSTATUS(child_status);
            if (command_index == num_commands - 1) {
                if (rc == 0) {
                    last_command_status = OK;
                } else if (rc == ENOENT) {
                    printf("error: file/directory/command not found\n");
                    last_command_status = rc;
                } else if (rc == EACCES) {
                    printf("error: permission denied\n");
                    last_command_status = rc;
                } else {
                    printf("Failed to execute command with rc %d\n", rc);
                    last_command_status = rc;
                }
            }
        } else {
            printf(CMD_ERR_EXECUTE);
            last_command_status = ERR_EXEC_CMD;
        }
    }

    return last_command_status;
}



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


int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];
    command_list_t cmd_list;
    cmd_list.num = 0;  
    int rc = OK;

    while (1) {
        fprintf(stdout, "%s", SH_PROMPT);

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            fprintf(stdout, "\n");
            break;
        }

        if (strchr(cmd_buff, '\n') == NULL) {
            fprintf(stderr, "error: maximum buffer size for user input is %d\n", SH_CMD_MAX);
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            continue;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strlen(cmd_buff) == 0) {
            continue;
        }

        char _cmd_buff[SH_CMD_MAX];
        strncpy(_cmd_buff, cmd_buff, SH_CMD_MAX);
        _cmd_buff[SH_CMD_MAX - 1] = '\0';  

        rc = build_cmd_list(_cmd_buff, &cmd_list);

        if (rc != OK) {
            if (rc == WARN_NO_CMDS) {
                fprintf(stderr, CMD_WARN_NO_CMD);
            } else if (rc == ERR_TOO_MANY_COMMANDS) {
                fprintf(stderr, CMD_ERR_PIPE_LIMIT, CMD_MAX);
            } else if (rc == ERR_CMD_OR_ARGS_TOO_BIG) {
                fprintf(stderr, "ERR_CMD_OR_ARGS_TOO_BIG", CMD_MAX);
            } else if (rc == ERR_CMD_ARGS_BAD) {
                fprintf(stderr, "ERR_CMD_ARGS_BAD", CMD_MAX);
            } else if (rc == ERR_MEMORY) {
                fprintf(stderr, "ERR_MEMORY", CMD_MAX);
            } else if (rc == ERR_EXEC_CMD) {
                fprintf(stderr, "ERR_EXEC_CMD", CMD_MAX);
            } else if (rc == OK_EXIT) {
                fprintf(stderr, "OK_EXIT", CMD_MAX);
            } else {
                fprintf(stderr, "error parsing command line\n");
            }
            free_cmd_list(&cmd_list);  
            continue;
        }

        if (cmd_list.num == 1) {
            Built_In_Cmds bi = exec_built_in_cmd(&cmd_list.commands[0]);
            if (bi == BI_CMD_EXIT) {
                rc = OK;
                                printf("exiting...");
                                free_cmd_list(&cmd_list);
                break;
            } else if (bi == BI_NOT_BI) {
                rc = exec_cmd(&cmd_list.commands[0]);
            }
        } else {
            rc = exec_pipeline(&cmd_list);
        }

        last_rc = rc;
        free_cmd_list(&cmd_list);  
    }
    return rc;
}
