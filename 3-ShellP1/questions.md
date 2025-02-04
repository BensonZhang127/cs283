1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets is a good choice because we want to read line by line and have the entire line until a newline, EOF, or exceeds the buffer. This is a good choice for our application because we discussed how the buffer actually tracks everything the user is inputting like "backspace" and "arrow keys" until the user hits "enter". It also allows us to run the program headless for testing and checking return code of fgets.  

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Allocated memory on the heap with malloc is often better than allocating a fixed sized array because it is more flexible and efficient when we don't know the size of the input from the user. Also since the memory is allocated on the heap which is larger than the stack, it makes it easier for future modification when sizes change and when malloc-ing we can detect if we fail to allocate memory easier by checking if it returned NULL or not, which compared to the stack don't have a way to detect it. 

3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming spaces is necessary because with extra white space we may treat them as commands which isn't valid, take them in as arguments, mess with tokenization, or other issues with parsing and executing the commands correctly.  

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  3 examples of redirection are output redirection(>), input rediction(<), and append redirection(>>). An example of output redirection is (ls > someFile.txt), which will send the output of ls into the new text file. Some challenges we may face implementing output redirection is that we will have to handle the case where the text file we want to redirect output towards, may already exists. For the input redirection, an example is (sort < someFile.txt), and some challenges are checking if the file exists and accessing the file. Lastly, for append redirection, like (echo "something" >> someFile.txt), the challenges are similar to input redirection since we still need to check if the file exists or if it is accessible.      

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Key differences between redirection and piping is that redirections are mostly used for files, while pipes are mostly used for glueing commands, which are easier to be chained and have a long pipe command. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  This is important for organization purposes. For instance, this allows easier error handling because we know we can check STDERR for errors and STDOUT when we have no errors. Because of this we are also allowed to redirect errors to logs while displaying normal outputs. Essentially, would you rather look at your screen filled with errors and expected outputs, or would you rather have some control/knowledge of knowing that errors and regular outputs are separated into STDERR and STDOUT.    

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Like the would you rather scenario I mentioned for the previous question, our custom shell should separate errors from commands into STDERR and non-errors into STDOUT. We should also provide ways to merge STDOUT and STDERR to not put limitations for the users, and in case of warnings, which we will accomplish most likely with redirections.  