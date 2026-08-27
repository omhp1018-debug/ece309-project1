#!/bin/bash

# Compile harness.c
gcc harness.c -o harness

if [ $? -ne 0 ]; then
    echo "FAIL: Compile"
    exit 1
else
    echo "PASS: Compile"
fi


# Test 1: hello response
output=$(printf "hello\nexit\n" | ./harness)

if echo "$output" | grep -q "Hello! Nice to meet you!"; then
    echo "PASS: Hello response"
else
    echo "FAIL: Hello response"
fi


# Test 2: calculator
output=$(printf "calc 10 + 5\nexit\n" | ./harness)

if echo "$output" | grep -q "Assistant: 15"; then
    echo "PASS: Calculator"
else
    echo "FAIL: Calculator"
fi


# Test 3: division by zero
output=$(printf "calc 10 / 0\nexit\n" | ./harness)

if echo "$output" | grep -q "Error: cannot divide by zero."; then
    echo "PASS: Division by zero"
else
    echo "FAIL: Division by zero"
fi


# Test 4: history keeps only the newest 5 turns
output=$(printf "message1\nmessage2\nmessage3\nmessage4\nmessage5\nmessage6\nhistory\nexit\n" | ./harness)

# Only look at the section printed by the history command.
history_output=$(echo "$output" | sed -n '/Conversation History:/,$p')

history_pass=true

# message1 should have been removed.
if echo "$history_output" | grep -q "You: message1"; then
    history_pass=false
fi

# message2 through message6 should still be stored.
for n in 2 3 4 5 6
do
    if ! echo "$history_output" | grep -q "You: message$n"; then
        history_pass=false
    fi
done

if [ "$history_pass" = true ]; then
    echo "PASS: 5-turn history"
else
    echo "FAIL: 5-turn history"
fi


# Test 5: exit command
output=$(printf "exit\n" | ./harness)

if echo "$output" | grep -q "Goodbye!"; then
    echo "PASS: Exit command"
else
    echo "FAIL: Exit command"
fi


# Test 6: basic memory leak check
# Use 6 messages so the oldest history entry must be freed.
memory_input="one
two
three
four
five
six
exit
"

if command -v leaks >/dev/null 2>&1; then

    output=$(printf "%s" "$memory_input" | leaks --atExit -- ./harness 2>&1)

    if echo "$output" | grep -q "0 leaks for 0 total leaked bytes"; then
        echo "PASS: Memory leaks"
    else
        echo "FAIL: Memory leaks"
    fi

elif command -v valgrind >/dev/null 2>&1; then

    output=$(printf "%s" "$memory_input" | valgrind --leak-check=full ./harness 2>&1)

    if echo "$output" | grep -q "definitely lost: 0 bytes"; then
        echo "PASS: Memory leaks"
    else
        echo "FAIL: Memory leaks"
    fi

else
    echo "SKIP: Memory leaks (leaks/valgrind not installed)"
fi


echo "Testing complete."