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
    expected_output="dsh2>cmdloopreturned-7"


    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
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
    expected_output="/tmp/someDirdsh2>dsh2>dsh2>cmdloopreturned0"
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
    expected_output="${current_dir}dsh2>error:couldnotchangedirectorytogibberishdsh2>dsh2>cmdloopreturned0"
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
    expected_output="${current_dir}dsh2>error:toomanyargumentsforcddsh2>dsh2>cmdloopreturned0"
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
    expected_output="hello worlddsh2> dsh2> cmd loop returned 0"

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
    expected_output="${current_dir}dsh2>error:toomanyargumentsforcddsh2>dsh2>cmdloopreturned0"
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
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

    [[ "$stripped_output" == "dsh2>@%%%%"* ]]
    [[ "$stripped_output" == *"%%%%%%%@dsh2>cmdloopreturned0" ]]
    [ "$status" -eq 0 ]
}
