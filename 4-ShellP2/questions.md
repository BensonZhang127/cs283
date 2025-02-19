1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  The common fork before exec pattern is used because we might not want to replace the current process with another. By using fork before execvp won't replace the current process with a new one, which means our original process will not disappear and can continue running after child process creation. We use fork so that we can create a child process to execute a new program while the original parent process won't be affected. 

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() system call fails, then it means our creation of the new copy process failed, which might be due to resource/memory errors. My implementation handles this scenario by stopping the execution of the command to print CMD_ERR_EXECUTE and return ERR_EXEC_CMD, then we will wait for the next command.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for the executable file specified by cmd->argv[0] in the directories listed in the PATH environment variable.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The first thing that comes to mind is zombie processes or race conditions, since if we don't tell the parent process to wait, then they both will run, and our demos has showed issues with that of either the child keeps running or nondeterministic behavior that is also hard to spot race conditions as the program gets larger. 
    
5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() extract the child's exit code when the child process terminated normally. This exit code is important because it's an indicator for us to determine whether the child process succeeded or encountered an error. Which then we can use to determine how to handle the error like printing what error message.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  As discussed in class, we basically have a new data structure that is parsing/slitting input into tokens that are themselves null terminated. Then we talk about an "in_quote_mode" flag to basically toggle if we are inside of a quote or not, since we need to capture the entire string including spaces when we are dealing with quotes. This is necessary because some commands allow arguments with spaces or quotes, and also the functions like execvp() expect strings to be null terminated.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I feel like I made a large amount of changes in my parsing logic compared to the previous assignment, even though they were somewhat similar. I don't really think there an any "unexpected" challenges as I already knew from looking at the assignment, that this is going to take a while. Since generally, a data structure change require a lot of refactoring. Also, generally, I think splitting by pipes or spaces, are quite similar so it wasn't too difficult. The small discussion in lecture about the new data structure and "in_quote_mode" was also a bit helpful. Having our own bats/student_test.sh file was new, and a way of testing that I have not experienced before so that was kind of cool. Overall, I would say not really any unexpected challenges mainly because I already expected a high difficulty assignment. 

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  The purpose of signals in a Linux system is that they are used for notifying processes of events. The events include exceptions, interruptions, or system events like a child process termination. Signals allow asynchronous notification, like a process receiving a death signal to stop execution. This is differencet from other forms of IPC, like pipes, which involve explicit data exchange and synchronization between processes. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  1. SIGKILL is a kill signal. It cannot be cannot be caught, blocked, or ignored (along with SIGSTOP). And it is used to force a process to end. 2. SIGTERM is a termination signal. It is the default termination signal sent by the kill command and it requests a process to terminate gracefully. Processes can catch and handle this signal, allowing them to save or free resources before exiting. 3. SIGINT is the interrupt from keyboard signal, so this is the commonly known CTRL+C. It is used to request a process to terminate gracefully, which also allows cleanup operations afterwards.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process recieves the SIGSTOP signal, they pause and are moved to a stopped state. It cannot be caught or ignored like SIGINT, and is similar to SIGKILL. It cannot be caught or ignored since the whole point of it is to stop a process immediately for debugging, job control, or other system administration purposes.   
    