#!/usr/bin/env bats

# File: student_tests.sh
#
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}


@test "ls -l returns some output (non-empty)" {
  run ./dsh <<EOF
ls -l
exit
EOF
  [ -n "$output" ]
  [ "$status" -eq 0 ]
}

@test "exit command terminates shell" {
    run ./dsh <<EOF
exit
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="localmodedsh4>exiting...cmdloopreturned0"


    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Comparison:"
    echo "Stripped: ${stripped_output}"
    echo "Expected: ${expected_output}"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "cd with 1 argument" {
    current_dir=$(pwd)
    cd /tmp
    mkdir -p someDir
    run "${current_dir}/dsh" <<EOF
cd someDir
pwd
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/tmp/someDirlocalmodedsh4>dsh4>dsh4>cmdloopreturned0"
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "cd bad argument (should not change directory)" {
    current_dir=$(pwd)

    run "${current_dir}/dsh" <<EOF
cd gibberish
pwd
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="${current_dir}localmodedsh4>error:couldnotchangedirectorytogibberishdsh4>dsh4>cmdloopreturned0"
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Output stripped: ${stripped_output}"
    echo "Expected       : ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "cd too many arguments" {
    current_dir=$(pwd)

    run "${current_dir}/dsh" <<EOF
cd dsh_test bats
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="${current_dir}localmodedsh4>error:toomanyargumentsforcddsh4>dsh4>cmdloopreturned0"
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Output stripped: ${stripped_output}"
    echo "Expected       : ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "sample uname" {
    run ./dsh <<EOF
uname
exit
EOF
    [[ "$output" == *"Linux"* ]]
    [ "$status" -eq 0 ]
}

@test "echo no quote" {
    run "./dsh" <<EOF
   echo "hello world"
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output="hello worldlocal modedsh4> dsh4> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Comparison:"
    echo "Stripped: ${stripped_output}"
    echo "Expected: ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "pwd should print current directory" {
    current_dir=$(pwd)
    run ./dsh <<EOF
pwd
exit
EOF
    [[ "$output" == *"$current_dir"* ]]
    [ "$status" -eq 0 ]
}

@test "ENOENT error message" {
    run ./dsh <<EOF
gibberish
exit
EOF
    # Check that the error message gets returned
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="${current_dir}localmodedsh4>error:toomanyargumentsforcdlocalmodedsh4>dsh4>cmdloopreturned0"
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Comparison:"
    echo "Output stripped: ${stripped_output}"
    echo "Expected       : ${expected_output}"

    [[ "$output" == *"file/directory/command not found"* ]]
    [ "$status" -eq 0 ]
}

@test "EACCES error message" {
    current_dir=$(pwd)
    touch "$current_dir"/test_file
    chmod 000 "$current_dir"/test_file
    run ./dsh <<EOF
${current_dir}/test_file
exit
EOF
    # Check that the error message gets returned
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="error: permission denied"
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Output stripped: ${stripped_output}"
    echo "Expected       : ${expected_output}"
    [[ "$output" == *"error: permission denied"* ]]
    [ "$status" -eq 0 ]
    rm -rf "$current_dir"/test_file
}


@test "Other errno" {
    run ./dsh <<EOF
false
exit
EOF
    # Check that the error message gets returned
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Failed to execute command with rc 1"
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "Output stripped: ${stripped_output}"
    echo "Expected       : ${expected_output}"
    [[ "$output" == *"Failed to execute command with rc 1"* ]]
    [ "$status" -eq 0 ]
}


@test "front and end of dragon command output" {
    run ./dsh <<EOF
dragon
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    [[ "$stripped_output" == "localmodedsh4>@%%%%"* ]]
    [[ "$stripped_output" == *"%%%%%%%@dsh4>cmdloopreturned0" ]]
    [ "$status" -eq 0 ]
}



@test "Exceed Pipe Limit" {
    run "./dsh" <<EOF
echo | echo | echo | echo | echo | echo | echo | echo | echo 9
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="error:pipinglimitedto8commandslocalmodedsh4>dsh4>cmdloopreturned-2"

    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}



@test "Empty After Pipe" {
    run "./dsh" <<EOF
echo 1 |
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>cmdloopreturned-6"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Empty in between 2 pipe" {
    run "./dsh" <<EOF
echo 1 | | echo 3
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="error:invalidcommandinpipelinelocalmodedsh4>dsh4>cmdloopreturned-6"
        echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Command start with pipe is not allowed" {
    run "./dsh" <<EOF
| echo 2
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>cmdloopreturned-6"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Starting with pipe is not allowed" {
    run "./dsh" <<EOF
|
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>cmdloopreturned-6"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "Multiple piping commands work" {
    run "./dsh" <<EOF
echo hello | grep hello | wc -l
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="1localmodedsh4>dsh4>cmdloopreturned0"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "White space does not affect pipes" {
    run "./dsh" <<EOF
echo hello            |      grep hello    | wc -l
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="1localmodedsh4>dsh4>cmdloopreturned0"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "Ending with pipe is not allowed" {
    run "./dsh" <<EOF
echo hello |
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>cmdloopreturned-6"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}




@test "Piping with arguments works" {
    run "./dsh" <<EOF
echo hello | tr a-z A-Z
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="HELLOlocalmodedsh4>dsh4>cmdloopreturned0"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "output redirection using >" {
    run ./dsh <<EOF
echo "gibberish" > temp_redir_file.txt
cat temp_redir_file.txt
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="gibberishlocalmodedsh4>dsh4>dsh4>cmdloopreturned0"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f temp_redir_file.txt
}



@test "append redirection using >>" {
    run ./dsh <<EOF
echo "gibberish1" > some_temp.txt
echo "gibberish2" >> some_temp.txt
cat some_temp.txt
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="gibberish1gibberish2localmodedsh4>dsh4>dsh4>dsh4>cmdloopreturned0"

    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]

    rm -f some_temp.txt
}

@test "pipe with output redirection" {
    run ./dsh <<EOF
echo "gibberish" | tr a-z A-Z > some_temp.txt
cat some_temp.txt
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="GIBBERISHlocalmodedsh4>dsh4>dsh4>cmdloopreturned0"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "input redirection using <" {
    echo "gibberish" > some_temp.txt
    run ./dsh <<EOF
cat < some_temp.txt
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="gibberishlocalmodedsh4>dsh4>cmdloopreturned0"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "pipe with input and output redirection" {
    echo "gibberish" > some_temp_in.txt

    run ./dsh <<EOF
cat < some_temp_in.txt | tr a-z A-Z > some_temp_out.txt
cat some_temp_out.txt
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="GIBBERISHlocalmodedsh4>dsh4>dsh4>cmdloopreturned0"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp_in.txt some_temp_out.txt
}


@test "output redirection using > (missing output)" {
    run ./dsh <<EOF
echo "gibberish" >
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>cmdloopreturned-4"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}


@test "output redirection using >> (missing output)" {
    run ./dsh <<EOF
echo "gibberish1" > some_temp.txt
echo "gibberish2" >>
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>dsh4>cmdloopreturned-4"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "input redirection using < (without input)" {
    echo "gibberish" > some_temp.txt
    run ./dsh <<EOF
cat <
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>cmdloopreturned-4"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}


@test "output redirection using > (missing input)" {
    run ./dsh <<EOF
> some_temp.txt
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>cmdloopreturned-4"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}


@test "output redirection using >> (missing input)" {
    run ./dsh <<EOF
echo "gibberish1" > some_temp.txt
>> some_temp.txt
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>dsh4>cmdloopreturned-4"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "input redirection using < (without output)" {
    echo "gibberish" > some_temp.txt
    run ./dsh <<EOF
< some_temp.txt
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="errorparsingcommandlinelocalmodedsh4>dsh4>cmdloopreturned-4"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "Help flag: ./dsh -h" {
    run ./dsh -h

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Usage:./dsh[-c|-s][-iIP][-pPORT][-x][-h]Defaultistorun./dshinlocalmode-cRunasclient-sRunasserver-iIPSetIP/Interfaceaddress(onlyvalidwith-cor-s)-pPORTSetportnumber(onlyvalidwith-cor-s)-xEnablethreadedmode(onlyvalidwith-s)-hShowthishelpmessage"
    echo "Comparison:"
    echo "Stripped: ${stripped_output}"
    echo "Expected: ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "client-server: server didn't start, so client fails" {
    run ./dsh -c -p 4086 <<EOF
echo "gibberish"
EOF
    echo "Output: $output"
    [[ "$output" =~ "Connection refused" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: Server start and Client Connect on default port" {
    ./dsh -s &
    server_pid=$!
    sleep 1
    run ./dsh -c <<EOF
EOF

    kill $server_pid
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:7127Connectedto127.0.0.1:7127rsh>Inputendedcmdloopreturned0"
    echo "Comparison:"
    echo "Stripped: ${stripped_output}"
    echo "Expected: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "client-server P flag: Server Client Connect on specific Port" {
    ./dsh -s -p 3069 &
    server_pid=$!
    sleep 1

    run ./dsh -c -p 3069 <<EOF
EOF

    kill $server_pid
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:3069Connectedto127.0.0.1:3069rsh>Inputendedcmdloopreturned0"
    echo "Comparison:"
    echo "Stripped: ${stripped_output}"
    echo "Expected: ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "client-server: exit client, server don't stop" {
    ./dsh -s -p 3070 &
    server_pid=$!
    sleep 1

    run ./dsh -c -p 3070 <<EOF
exit
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:3070Connectedto127.0.0.1:3070rsh>exiting...cmdloopreturned0"
    echo "Comparison:"
    echo "Stripped: ${stripped_output}"
    echo "Expected: ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]

    run ./dsh -c -p 3070 <<EOF
echo "gibberish"
exit
EOF

    kill $server_pid

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:3070Connectedto127.0.0.1:3070rsh>gibberishrsh>exiting...cmdloopreturned0"
    echo "Comparison:"
    echo "Stripped: ${stripped_output}"
    echo "Expected: ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "client-server: stop-server command" {
    ./dsh -s -p 3071 &
    server_pid=$!
    sleep 1

    run ./dsh -c -p 3071 <<EOF
stop-server
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:3071Connectedto127.0.0.1:3071rsh>exiting...cmdloopreturned0"
    echo "Comparison:"
    echo "Stripped: ${stripped_output}"
    echo "Expected: ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]

    run ./dsh -c -p 3071 <<EOF
echo "server already closed"
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="connect:Connectionrefusedsocketclientmode:addr:127.0.0.1:3071Failedtoconnectto127.0.0.1:3071cmdloopreturned-52"
    echo "Comparison:"
    echo "Stripped: ${stripped_output}"
    echo "Expected: ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "client-server: cd with no argument (home directory)" {
    ./dsh -s -p 4074 &
    server_pid=$!
    sleep 1

    run ./dsh -c -p 4074 <<EOF
cd
pwd
EOF
    kill $server_pid

    echo "Output: $output"

    [[ "$output" =~ "cd: missing argument" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: cd with non-existent directory" {
    ./dsh -s -p 4075 &
    server_pid=$!
    sleep 1

    run ./dsh -c -p 4075 <<EOF
cd gibberish
pwd
EOF
    kill $server_pid

    echo "Output: $output"

    [[ "$output" =~ "error: could not change directory to gibberish" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: cd with 1 argument" {
  ./dsh -s -p 4076 &
  server_pid=$!
  sleep 1
  pwd=$(pwd)

  output=$(timeout 10s ./dsh -c -p 4076 <<EOF
cd bats
pwd
EOF
  )

  if kill -0 $server_pid 2>/dev/null; then
    timeout 10s kill $server_pid
  fi
  echo "OUTPUT: $output"
  echo "EXPECT: $pwd/bats"

  [[ "$output" =~ "/bats" ]]
}

@test "client-server: ls" {
    ./dsh -s -p 4077 &
    server_pid=$!
    sleep 1
    expected_output=$(ls)
    run ./dsh -c -p 4077 <<EOF
ls
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi

    echo "Output: $output"

    [[ "$output" =~ "$expected_output" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: ls with flag" {
    ./dsh -s -p 4078 &
    server_pid=$!
    sleep 1
    expected_output=$(ls -l)
    run ./dsh -c -p 4078 <<EOF
ls -l
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi

    echo "Output: $output"

    [[ "$output" =~ "$expected_output" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: custom dragon" {
    ./dsh -s -p 4079 &
    server_pid=$!
    sleep 1
    expected_output=$(cat dragon.txt)
    run ./dsh -c -p 4079 <<EOF
dragon
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi

    echo "Output: $output"

    [[ "$output" =~ "$expected_output" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: pwd" {
    ./dsh -s -p 4080 &
    server_pid=$!
    sleep 1
    expected_output=$(pwd)
    run ./dsh -c -p 4080 <<EOF
pwd
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "$expected_output" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: uname" {
    ./dsh -s -p 4081 &
    server_pid=$!
    sleep 1
    expected_output=$(uname)
    run ./dsh -c -p 4081 <<EOF
uname
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "$expected_output" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: pipe test" {
    ./dsh -s -p 4082 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4082 <<EOF
echo "gibberish" | tr a-z A-Z
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "GIBBERISH" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: pipe missing input" {
    ./dsh -s -p 4083 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4083 <<EOF
| tr a-z A-Z
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "error parsing command line" ]]
    [ "$status" -eq 0 ]

}

@test "client-server: pipe missing command" {
    ./dsh -s -p 4084 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4084 <<EOF
echo "gibberish" |
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "error parsing command line" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: pipe exceed max" {
    ./dsh -s -p 4085 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4085 <<EOF
echo "gibberish" | echo 1 | echo 2 | echo 3 | echo 4 | echo 5 | echo 6 | echo 7 | echo 8 | echo 9
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "error: piping limited to 8 commands" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: 2 servers of the same port" {
    ./dsh -s -p 4086 &
    server_pid=$!
    sleep 1
    run ./dsh -s -p 4086
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "Address already in use" ]]
    [ "$status" -eq 0 ]

}

@test "client-server: output redirection using >" {
    ./dsh -s -p 4087 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4087 <<EOF
echo "gibberish" > temp_redir_file.txt
cat temp_redir_file.txt
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "gibberish" ]]
    [ "$status" -eq 0 ]
    rm -f temp_redir_file.txt
}

@test "client-server: append redirection using >>" {
    ./dsh -s -p 4088 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4088 <<EOF
echo "gibberish1" > some_temp.txt
echo "gibberish2" >> some_temp.txt
cat some_temp.txt
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "gibberish1" ]]
    [[ "$output" =~ "gibberish2" ]]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "client-server: input redirection using <" {
    ./dsh -s -p 4089 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4089 <<EOF
echo "gibberish" > some_temp.txt
cat < some_temp.txt
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "gibberish" ]]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "client-server: pipe with output redirection" {
    ./dsh -s -p 4090 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4090 <<EOF
echo "gibberish" | tr a-z A-Z > some_temp.txt
cat some_temp.txt
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "GIBBERISH" ]]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "client-server: pipe with input and output redirection" {
    ./dsh -s -p 4091 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4091 <<EOF
echo "gibberish" > some_temp.txt
cat < some_temp.txt | tr a-z A-Z > some_temp_out.txt
cat some_temp_out.txt
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "GIBBERISH" ]]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt some_temp_out.txt

}

@test "client-server: output redirection using > (missing output)" {
    ./dsh -s -p 4092 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4092 <<EOF
echo "gibberish" >
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "error parsing command line" ]]
    [ "$status" -eq 0 ]
}

@test "client-server: append redirection using >> (missing output)" {
    ./dsh -s -p 4093 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4093 <<EOF
echo "gibberish1" > some_temp.txt
echo "gibberish" >>
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "error parsing command line" ]]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "client-server: input redirection using < (missing input)" {
    ./dsh -s -p 4094 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p 4094 <<EOF
echo "gibberish" > some_temp.txt
cat <
EOF
    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi
    echo "Output: $output"
    [[ "$output" =~ "error parsing command line" ]]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}

@test "multi-threaded client-server: -x flag" {
    ./dsh -s -p 4095 -x &
    server_pid=$!
    sleep 1

    client1_output=$(./dsh -c -p 4095 <<INNER_EOF
echo "gibberish 1"
exit
INNER_EOF
)
    client1_status=$?

    client2_output=$(./dsh -c -p 4095 <<INNER_EOF
echo "gibberish 2"
exit
INNER_EOF
)
    client2_status=$?

    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi

    echo "Client 1 Output: $client1_output"
    echo "Client 2 Output: $client2_output"

    [[ "$client1_output" =~ "gibberish 1" ]]
    [[ "$client2_output" =~ "gibberish 2" ]]
    [ "$client1_status" -eq 0 ]
    [ "$client2_status" -eq 0 ]
}

@test "multi-threaded client-server: cd don't affect other clients" {
    ./dsh -s -p 4096 -x &
    server_pid=$!
    sleep 1

    expected_output1=$(ls)
    expected_output2=$(cd ..; ls)

    client1_output=$(./dsh -c -p 4096 <<INNER_EOF
ls
exit
INNER_EOF
)
    client1_status=$?

    client2_output=$(./dsh -c -p 4096 <<INNER_EOF
cd ..
ls
exit
INNER_EOF
)
    client2_status=$?

    if kill -0 $server_pid 2>/dev/null; then
        kill $server_pid
    fi

    echo "Client 1 Output:"
    echo "$client1_output"
    echo "Expected Output 1:"
    echo "$expected_output1"
    echo "Client 2 Output:"
    echo "$client2_output"
    echo "Expected Output 2:"
    echo "$expected_output2"

    [[ "$client1_output" =~ $expected_output1 ]]
    [[ "$client2_output" =~ $expected_output2 ]]
    [ "$client1_status" -eq 0 ]
    [ "$client2_status" -eq 0 ]
}