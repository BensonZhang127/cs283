#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include "dshlib.h"
#include "rshlib.h"



void *handle_client(void *arg) {
    client_data_t *data = (client_data_t *)arg;
    int cli_socket = data->cli_socket;
    int rc = process_client_request(cli_socket, data->cwd);
    close(cli_socket);
    printf("Client disconnected from %d, rc=%d\n", cli_socket, rc);
    free(data);
    pthread_exit(NULL);
}

/*
 * start_server(ifaces, port, is_threaded)
 *      ifaces:  a string in ip address format, indicating the interface
 *              where the server will bind.  In almost all cases it will
 *              be the default "0.0.0.0" which binds to all interfaces.
 *              note the constant RDSH_DEF_SVR_INTFACE in rshlib.h
 * 
 *      port:   The port the server will use.  Note the constant 
 *              RDSH_DEF_PORT which is 1234 in rshlib.h.  If you are using
 *              tux you may need to change this to your own default, or even
 *              better use the command line override -s implemented in dsh_cli.c
 *              For example ./dsh -s 0.0.0.0:5678 where 5678 is the new port  
 * 
 *      is_threded:  Used for extra credit to indicate the server should implement
 *                   per thread connections for clients  
 * 
 *      This function basically runs the server by: 
 *          1. Booting up the server
 *          2. Processing client requests until the client requests the
 *             server to stop by running the `stop-server` command
 *          3. Stopping the server. 
 * 
 *      This function is fully implemented for you and should not require
 *      any changes for basic functionality.  
 * 
 *      IF YOU IMPLEMENT THE MULTI-THREADED SERVER FOR EXTRA CREDIT YOU NEED
 *      TO DO SOMETHING WITH THE is_threaded ARGUMENT HOWEVER.  
 */
int start_server(char *ifaces, int port, int is_threaded) {
    int svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0) {
        return svr_socket;
    }

    printf("Server initial cwd: %s\n", getcwd(NULL, 0));

    int rc = OK;
    if (is_threaded) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        char initial_cwd[MAX_PATH_SIZE];
        if (getcwd(initial_cwd, MAX_PATH_SIZE) == NULL) {
            perror("getcwd");
            strcpy(initial_cwd, "/");
        }

        while (1) {
            printf("Waiting for client connection...\n");
            int cli_socket = accept(svr_socket, (struct sockaddr*)&client_addr, &addrlen);
            if (cli_socket < 0) {
                perror("accept");
                rc = ERR_RDSH_COMMUNICATION;
                break;
            }
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            printf("Client connected from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

            client_data_t *data = malloc(sizeof(client_data_t));
            if (!data) {
                perror("malloc client data");
                close(cli_socket);
                continue;
            }
            data->cli_socket = cli_socket;
            strncpy(data->cwd, initial_cwd, MAX_PATH_SIZE - 1);
            data->cwd[MAX_PATH_SIZE - 1] = '\0';

            pthread_t thread;
            if (pthread_create(&thread, NULL, handle_client, data) != 0) {
                perror("pthread_create");
                close(cli_socket);
                free(data);
            } else {
                pthread_detach(thread);
            }
        }
    } else {
        rc = process_cli_requests(svr_socket);
    }

    stop_server(svr_socket);
    return rc;
}
/*
 * stop_server(svr_socket)
 *      svr_socket: The socket that was created in the boot_server()
 *                  function. 
 * 
 *      This function simply returns the value of close() when closing
 *      the socket.  
 */
int stop_server(int svr_socket) {
    return close(svr_socket);
}
 
/*
 * boot_server(ifaces, port)
 *      ifaces & port:  see start_server for description.  They are passed
 *                      as is to this function.   
 * 
 *      This function "boots" the rsh server.  It is responsible for all
 *      socket operations prior to accepting client connections.  Specifically: 
 * 
 *      1. Create the server socket using the socket() function. 
 *      2. Calling bind to "bind" the server to the interface and port
 *      3. Calling listen to get the server ready to listen for connections.
 * 
 *      after creating the socket and prior to calling bind you might want to 
 *      include the following code:
 * 
 *      int enable=1;
 *      setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
 * 
 *      when doing development you often run into issues where you hold onto
 *      the port and then need to wait for linux to detect this issue and free
 *      the port up.  The code above tells linux to force allowing this process
 *      to use the specified port making your life a lot easier.
 * 
 *  Returns:
 * 
 *      server_socket:  Sockets are just file descriptors, if this function is
 *                      successful, it returns the server socket descriptor, 
 *                      which is just an integer.
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code is returned if the socket(),
 *                               bind(), or listen() call fails. 
 * 
 */
int boot_server(char *ifaces, int port) {
    int svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket < 0) {
        perror("socket");
        return ERR_RDSH_COMMUNICATION;
    }
    int enable = 1;
    if (setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ifaces);
    if (bind(svr_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
    if (listen(svr_socket, 5) < 0) {
        perror("listen");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
    printf("Server started on %s:%d\n", ifaces, port);
    return svr_socket;
}

 
/*
 * process_cli_requests(svr_socket)
 *      svr_socket:  The server socket that was obtained from boot_server()
 *   
 *  This function handles managing client connections.  It does this using
 *  the following logic
 * 
 *      1.  Starts a while(1) loop:
 *  
 *          a. Calls accept() to wait for a client connection. Recall that 
 *             the accept() function returns another socket specifically
 *             bound to a client connection. 
 *          b. Calls exec_client_requests() to handle executing commands
 *             sent by the client. It will use the socket returned from
 *             accept().
 *          c. Loops back to the top (step 2) to accept connecting another
 *             client.  
 * 
 *          note that the exec_client_requests() return code should be
 *          negative if the client requested the server to stop by sending
 *          the `stop-server` command.  If this is the case step 2b breaks
 *          out of the while(1) loop. 
 * 
 *      2.  After we exit the loop, we need to cleanup.  Dont forget to 
 *          free the buffer you allocated in step #1.  Then call stop_server()
 *          to close the server socket. 
 * 
 *  Returns:
 * 
 *      OK_EXIT:  When the client sends the `stop-server` command this function
 *                should return OK_EXIT. 
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code terminates the loop and is
 *                returned from this function in the case of the accept() 
 *                function failing. 
 * 
 *      OTHERS:   See exec_client_requests() for return codes.  Note that positive
 *                values will keep the loop running to accept additional client
 *                connections, and negative values terminate the server. 
 * 
 */
int process_cli_requests(int svr_socket) {
    char initial_cwd[MAX_PATH_SIZE];
    if (getcwd(initial_cwd, MAX_PATH_SIZE) == NULL) {
        perror("getcwd");
        strcpy(initial_cwd, "/");
    }

    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int rc = OK;

    while (1) {
        printf("Waiting for client connection...\n");
        int cli_socket = accept(svr_socket, (struct sockaddr*)&client_addr, &addrlen);
        if (cli_socket < 0) {
            perror("accept");
            return ERR_RDSH_COMMUNICATION;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Client connected from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        rc = exec_client_requests(cli_socket);
        close(cli_socket);
        printf("Client disconnected, rc=%d\n", rc);

        if (chdir(initial_cwd) < 0) {
            perror("chdir reset");
            rc = ERR_RDSH_COMMUNICATION;
        } else {
            printf("Reset cwd to %s\n", initial_cwd);
        }

        if (rc == OK_EXIT) {
            break;
        }
    }
    return rc;
}
/*
 * exec_client_requests(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client
 *   
 *  This function handles accepting remote client commands. The function will
 *  loop and continue to accept and execute client commands.  There are 2 ways
 *  that this ongoing loop accepting client commands ends:
 * 
 *      1.  When the client executes the `exit` command, this function returns
 *          to process_cli_requests() so that we can accept another client
 *          connection. 
 *      2.  When the client executes the `stop-server` command this function
 *          returns to process_cli_requests() with a return code of OK_EXIT
 *          indicating that the server should stop. 
 * 
 *  Note that this function largely follows the implementation of the
 *  exec_local_cmd_loop() function that you implemented in the last 
 *  shell program deliverable. The main difference is that the command will
 *  arrive over the recv() socket call rather than reading a string from the
 *  keyboard. 
 * 
 *  This function also must send the EOF character after a command is
 *  successfully executed to let the client know that the output from the
 *  command it sent is finished.  Use the send_message_eof() to accomplish 
 *  this. 
 * 
 *  Of final note, this function must allocate a buffer for storage to 
 *  store the data received by the client. For example:
 *     io_buff = malloc(RDSH_COMM_BUFF_SZ);
 *  And since it is allocating storage, it must also properly clean it up
 *  prior to exiting.
 * 
 *  Returns:
 * 
 *      OK:       The client sent the `exit` command.  Get ready to connect
 *                another client. 
 *      OK_EXIT:  The client sent `stop-server` command to terminate the server
 * 
 *      ERR_RDSH_COMMUNICATION:  A catch all for any socket() related send
 *                or receive errors. 
 */

 int exec_client_requests(int cli_socket) {
    char cwd[MAX_PATH_SIZE];
    if (getcwd(cwd, MAX_PATH_SIZE) == NULL) {
        perror("getcwd");
        strcpy(cwd, "/");
    }
    return process_client_request(cli_socket, cwd);
}

/*
 * send_message_eof(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client

 *  Sends the EOF character to the client to indicate that the server is
 *  finished executing the command that it sent. 
 * 
 *  Returns:
 * 
 *      OK:  The EOF character was sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the EOF character. 
 */
int send_message_eof(int cli_socket) {
    int bytes_sent = send(cli_socket, &RDSH_EOF_CHAR, 1, 0);
    if (bytes_sent != 1) {
        perror("send eof");
        return ERR_RDSH_COMMUNICATION;
    }
    return OK;
}


/*
 * send_message_string(cli_socket, char *buff)
 *      cli_socket:  The server-side socket that is connected to the client
 *      buff:        A C string (aka null terminated) of a message we want
 *                   to send to the client. 
 *   
 *  Sends a message to the client.  Note this command executes both a send()
 *  to send the message and a send_message_eof() to send the EOF character to
 *  the client to indicate command execution terminated. 
 * 
 *  Returns:
 * 
 *      OK:  The message in buff followed by the EOF character was 
 *           sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the message followed by the EOF character. 
 */
int send_message_string(int cli_socket, char *buff) {
    int len = strlen(buff);
    int bytes_sent = send(cli_socket, buff, len, 0);
    if (bytes_sent != len) {
        perror("send string");
        return ERR_RDSH_COMMUNICATION;
    }
    return send_message_eof(cli_socket);
}


/*
 * rsh_execute_pipeline(int cli_sock, command_list_t *clist)
 *      cli_sock:    The server-side socket that is connected to the client
 *      clist:       The command_list_t structure that we implemented in
 *                   the last shell. 
 *   
 *  This function executes the command pipeline.  It should basically be a
 *  replica of the execute_pipeline() function from the last deliverable. 
 *  The only thing different is that you will be using the cli_sock as the
 *  main file descriptor on the first executable in the pipeline for STDIN,
 *  and the cli_sock for the file descriptor for STDOUT, and STDERR for the
 *  last executable in the pipeline.  See picture below:  
 * 
 *      
 *┌───────────┐                                                    ┌───────────┐
 *│ cli_sock  │                                                    │ cli_sock  │
 *└─────┬─────┘                                                    └────▲──▲───┘
 *      │   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐  │  │    
 *      │   │   Process 1  │     │   Process 2  │     │   Process N  │  │  │    
 *      │   │              │     │              │     │              │  │  │    
 *      └───▶stdin   stdout├─┬──▶│stdin   stdout├─┬──▶│stdin   stdout├──┘  │    
 *          │              │ │   │              │ │   │              │     │    
 *          │        stderr├─┘   │        stderr├─┘   │        stderr├─────┘    
 *          └──────────────┘     └──────────────┘     └──────────────┘   
 *                                                      WEXITSTATUS()
 *                                                      of this last
 *                                                      process to get
 *                                                      the return code
 *                                                      for this function       
 * 
 *  Returns:
 * 
 *      EXIT_CODE:  This function returns the exit code of the last command
 *                  executed in the pipeline.  If only one command is executed
 *                  that value is returned.  Remember, use the WEXITSTATUS()
 *                  macro that we discussed during our fork/exec lecture to
 *                  get this value. 
 */
int rsh_execute_pipeline(int cli_sock, command_list_t *clist, char *cwd) {
    return execute_pipeline_common(cli_sock, clist, cwd);
}

int process_client_request(int cli_socket, char *cwd) {
    char *io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!io_buff) {
        return ERR_MEMORY;
    }

    while (1) {
        int total_received = 0, recv_bytes, is_eof = 0;
        memset(io_buff, 0, RDSH_COMM_BUFF_SZ);
        while (!is_eof && total_received < RDSH_COMM_BUFF_SZ - 1) {
            recv_bytes = recv(cli_socket, io_buff + total_received, RDSH_COMM_BUFF_SZ - total_received - 1, 0);
            if (recv_bytes <= 0) {
                printf("Recv failed or client closed: %d\n", recv_bytes);
                free(io_buff);
                return recv_bytes < 0 ? ERR_RDSH_COMMUNICATION : OK;
            }
            total_received += recv_bytes;
            io_buff[total_received] = '\0';
            if (io_buff[total_received - 1] == RDSH_EOF_CHAR) {
                is_eof = 1;
                io_buff[total_received - 1] = '\0';
            }
        }
        printf("Received: %d bytes: [%s]\n", total_received, io_buff);

        if (strcmp(io_buff, "exit") == 0) {
            send_message_string(cli_socket, "exiting...\n");
            free(io_buff);
            return OK;
        }
        if (strcmp(io_buff, "stop-server") == 0) {
            send_message_string(cli_socket, "exiting...\n");
            free(io_buff);
            return OK_EXIT;
        }

        command_list_t cmd_list = {0};
        int build_status = build_cmd_list(io_buff, &cmd_list);

        if (build_status != OK) {
            char msg[100];

            if (build_status == ERR_TOO_MANY_COMMANDS) {
                sprintf(msg, CMD_ERR_PIPE_LIMIT, CMD_MAX);
            } else if (build_status == WARN_NO_CMDS) {
                sprintf(msg, CMD_WARN_NO_CMD);
            } else {
                sprintf(msg, "error parsing command line\n");
            }

            send_message_string(cli_socket, msg);
            send_message_eof(cli_socket);
        } else {
            Built_In_Cmds bi = match_command(cmd_list.commands[0].argv[0]);
            if (bi == BI_CMD_CD && cmd_list.num == 1) {
                if (cmd_list.commands[0].argc < 2) {
                    send_message_string(cli_socket, "cd: missing argument\n");
                    send_message_eof(cli_socket);
                } else {
                    char new_path[MAX_PATH_SIZE];
                    const char *arg = cmd_list.commands[0].argv[1];
                    if (arg[0] == '/') {
                        strncpy(new_path, arg, MAX_PATH_SIZE - 1);
                        new_path[MAX_PATH_SIZE - 1] = '\0';
                    } else {
                        size_t cwd_len = strlen(cwd);
                        size_t arg_len = strlen(arg);
                        if (cwd_len + 1 + arg_len >= MAX_PATH_SIZE) {
                            char msg[100];
                            sprintf(msg, "cd: path too long: %s/%s\n", cwd, arg);
                            send_message_string(cli_socket, msg);
                            send_message_eof(cli_socket);
                        } else {
                            strcpy(new_path, cwd);
                            strcat(new_path, "/");
                            strcat(new_path, arg);
                            pid_t pid = fork();
                            if (pid == 0) {
                                if (chdir(new_path) < 0) {
                                    char msg[100];
                                    sprintf(msg, "error: could not change directory to %s\n", arg);
                                    write(STDOUT_FILENO, msg, strlen(msg));
                                    exit(1);
                                }
                                exit(0);
                            } else if (pid > 0) {
                                int status;
                                waitpid(pid, &status, 0);
                                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                                    if (realpath(new_path, cwd) == NULL) {
                                        perror("realpath");
                                        send_message_string(cli_socket, "error resolving path\n");
                                    } else {
                                        send_message_string(cli_socket, "");
                                    }
                                } else {
                                    char msg[100];
                                    sprintf(msg, "error: could not change directory to %s\n", arg);
                                    send_message_string(cli_socket, msg);
                                }
                                send_message_eof(cli_socket);
                            } else {
                                send_message_string(cli_socket, "fork failed\n");
                                send_message_eof(cli_socket);
                            }
                        }
                    }
                }
            } else if (bi != BI_NOT_BI && cmd_list.num == 1) {
                pid_t pid = fork();
                if (pid == 0) {
                    if (chdir(cwd) < 0) {
                        perror("chdir in built-in");
                        exit(errno);
                    }
                    dup2(cli_socket, STDOUT_FILENO);
                    dup2(cli_socket, STDERR_FILENO);
                    int bi_rc = exec_built_in_cmd(&cmd_list.commands[0]);
                    exit(bi_rc);
                } else if (pid > 0) {
                    int status;
                    waitpid(pid, &status, 0);
                    send_message_eof(cli_socket);
                } else {
                    send_message_string(cli_socket, "fork failed\n");
                    send_message_eof(cli_socket);
                }
            } else {
                int rc = rsh_execute_pipeline(cli_socket, &cmd_list, cwd);
                if (rc != OK) {
                    char msg[100];
                    sprintf(msg, "execution failed: %d\n", rc);
                    send_message_string(cli_socket, msg);
                }
                send_message_eof(cli_socket);
            }
            free_cmd_list(&cmd_list);
        }
    }

    free(io_buff);
    return OK;
}

int execute_pipeline_common(int cli_sock, command_list_t *clist, char *cwd) {
    int num_commands = clist->num;
    if (num_commands <= 0) {
        return WARN_NO_CMDS;
    }

    pid_t pids[CMD_MAX] = {0};
    int pipefd[2];
    int prev_fd = cli_sock;
    int rc = OK;

    for (int i = 0; i < num_commands; i++) {
        if (i < num_commands - 1) {
            if (pipe(pipefd) < 0) {
                perror("pipe");
                return ERR_MEMORY;
            }
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) {
            if (cwd && chdir(cwd) < 0) {
                perror("chdir in pipeline");
                exit(errno);
            }
            if (i == 0) {
                if (dup2(cli_sock, STDIN_FILENO) < 0) {
                    perror("dup2 stdin");
                    exit(errno);
                }
            } else {
                if (dup2(prev_fd, STDIN_FILENO) < 0) {
                    perror("dup2 stdin");
                    exit(errno);
                }
                if (prev_fd != cli_sock) close(prev_fd);
            }

            if (i < num_commands - 1) {
                if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
                    perror("dup2 stdout");
                    exit(errno);
                }
                close(pipefd[0]);
                close(pipefd[1]);
            } else {
                if (dup2(cli_sock, STDOUT_FILENO) < 0) {
                    perror("dup2 stdout");
                    exit(errno);
                }
                if (dup2(cli_sock, STDERR_FILENO) < 0) {
                    perror("dup2 stderr");
                    exit(errno);
                }
            }

            handle_redirection(&clist->commands[i]);
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            char msg[100];
            snprintf(msg, sizeof(msg), "execvp failed: %s\n", strerror(errno));
            write(STDERR_FILENO, msg, strlen(msg));
            exit(errno);
        } else {
            if (i > 0 && prev_fd != cli_sock) {
                close(prev_fd);
            }
            if (i < num_commands - 1) {
                close(pipefd[1]);
                prev_fd = pipefd[0];
            }
        }
    }

    for (int i = 0; i < num_commands; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (i == num_commands - 1 && WIFEXITED(status)) {
            rc = WEXITSTATUS(status);
        }
    }
    return rc;
}
