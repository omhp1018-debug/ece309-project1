#!/bin/bash

echo "Compiling harness..."

gcc harness.c -o harness

if [ $? -ne 0 ]; then
    echo "FAIL: Program did not compile."
    exit 1
fi

echo "PASS: Program compiled."
echo


# -------------------------
# Test 1: Hello response
# -------------------------

output=$(printf "hello\nexit\n" | ./harness)

if echo "$output" | grep -q "Hello! How can I help you?"; then
    echo "PASS: Hello test"
else
    echo "FAIL: Hello test"
fi


# -------------------------
# Test 2: Calculator
# -------------------------

output=$(printf "calc 10 + 5\nexit\n" | ./harness)

if echo "$output" | grep -q "Calculator result: 15.00"; then
    echo "PASS: Calculator test"
else
    echo "FAIL: Calculator test"
fi


# -------------------------
# Test 3: Division by zero
# -------------------------

output=$(printf "calc 10 / 0\nexit\n" | ./harness)

if echo "$output" | grep -q "Cannot divide by zero"; then
    echo "PASS: Division by zero test"
else
    echo "FAIL: Division by zero test"
fi


# -------------------------
# Test 4: History limit
# -------------------------

output=$(printf "one\ntwo\nthree\nfour\nfive\nsix\nhistory\nexit\n" | ./harness)

if echo "$output" | grep -q "You: six" &&
   ! echo "$output" | grep -A20 "Conversation History" | grep -q "You: one"; then

    echo "PASS: History keeps last 5 turns"
else
    echo "FAIL: History test"
fi


# -------------------------
# Test 5: Exit command
# -------------------------

output=$(printf "exit\n" | ./harness)

if echo "$output" | grep -q "Goodbye!"; then
    echo "PASS: Exit test"
else
    echo "FAIL: Exit test"
fi

# -------------------------
# Test 6: Memory leaks
# -------------------------

if command -v leaks >/dev/null 2>&1; then
    output=$(printf "hello\ncalc 5 + 5\nexit\n" | leaks --atExit -- ./harness 2>&1)

    if echo "$output" | grep -q "0 leaks for 0 total leaked bytes"; then
        echo "PASS: Memory leak test"
    else
        echo "FAIL: Memory leak test"
    fi

elif command -v valgrind >/dev/null 2>&1; then
    output=$(printf "hello\ncalc 5 + 5\nexit\n" | valgrind --leak-check=full ./harness 2>&1)

    if echo "$output" | grep -q "no leaks are possible"; then
        echo "PASS: Memory leak test"
    elif echo "$output" | grep -q "All heap blocks were freed"; then
        echo "PASS: Memory leak test"
    else
        echo "FAIL: Memory leak test"
    fi

else
    echo "SKIP: No memory leak checker installed"
fi

echo
echo "Testing complete."