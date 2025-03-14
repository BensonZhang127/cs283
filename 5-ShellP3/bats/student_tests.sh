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
    expected_output="dsh3>exiting...cmdloopreturned0"


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
    expected_output="/tmp/someDirdsh3>dsh3>dsh3>cmdloopreturned0"
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]

    rm -rf someDir
}


@test "cd bad argument (should not change directory)" {
    current_dir=$(pwd)

    run "${current_dir}/dsh" <<EOF
cd gibberish
pwd
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="${current_dir}dsh3>error:couldnotchangedirectorytogibberishdsh3>dsh3>cmdloopreturned0"
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
    expected_output="${current_dir}dsh3>error:toomanyargumentsforcddsh3>dsh3>cmdloopreturned0"
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
    expected_output="hello worlddsh3> dsh3> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

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
    expected_output="${current_dir}dsh3>error:toomanyargumentsforcddsh3>dsh3>cmdloopreturned0"
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

    [[ "$stripped_output" == "dsh3>@%%%%"* ]]
    [[ "$stripped_output" == *"%%%%%%%@dsh3>cmdloopreturned0" ]]
    [ "$status" -eq 0 ]
}



@test "Exceed Pipe Limit" {
    run "./dsh" <<EOF
echo | echo | echo | echo | echo | echo | echo | echo | echo 9
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="error:pipinglimitedto8commandsdsh3>dsh3>cmdloopreturned-2"

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
    expected_output="errorparsingcommandlinedsh3>dsh3>cmdloopreturned-6"
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
    expected_output="error:invalidcommandinpipelinedsh3>dsh3>cmdloopreturned-6"
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
    expected_output="errorparsingcommandlinedsh3>dsh3>cmdloopreturned-6"
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
    expected_output="errorparsingcommandlinedsh3>dsh3>cmdloopreturned-6"
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
    expected_output="1dsh3>dsh3>cmdloopreturned0"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "White space does not afect pipes" {
    run "./dsh" <<EOF
echo hello            |      grep hello    | wc -l
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="1dsh3>dsh3>cmdloopreturned0"
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
    expected_output="errorparsingcommandlinedsh3>dsh3>cmdloopreturned-6"
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
    expected_output="HELLOdsh3>dsh3>cmdloopreturned0"
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
    expected_output="gibberishdsh3>dsh3>dsh3>cmdloopreturned0"
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
    expected_output="gibberish1gibberish2dsh3>dsh3>dsh3>dsh3>cmdloopreturned0"

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
    expected_output="GIBBERISHdsh3>dsh3>dsh3>cmdloopreturned0"
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
    expected_output="gibberishdsh3>dsh3>cmdloopreturned0"
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
    expected_output="GIBBERISHdsh3>dsh3>dsh3>cmdloopreturned0"
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
    expected_output="errorparsingcommandlinedsh3>dsh3>cmdloopreturned-4"
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
    expected_output="errorparsingcommandlinedsh3>dsh3>dsh3>cmdloopreturned-4"
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
    expected_output="errorparsingcommandlinedsh3>dsh3>cmdloopreturned-4"
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
    expected_output="errorparsingcommandlinedsh3>dsh3>cmdloopreturned-4"
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
    expected_output="errorparsingcommandlinedsh3>dsh3>dsh3>cmdloopreturned-4"
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
    expected_output="errorparsingcommandlinedsh3>dsh3>cmdloopreturned-4"
    echo "S : ${stripped_output}"
    echo "Ex: ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
    rm -f some_temp.txt
}