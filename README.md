ECE 309 Project 1 - LLM Mini-Harness in C

Om Panchal
ECE 309
Fall 2026

Project Description

This project implements a simple LLM-style agent harness in standard C. The program reads user input from the terminal and decides whether the request should be handled by a mock language model or by a calculator tool.

The harness also stores the five most recent conversation turns. Each turn contains the user's message and the assistant response. The program uses dynamic memory for the stored messages and frees that memory when old history is removed and before the program exits.

No real LLM API or external libraries are used.

Features

Terminal input loop using fgets()

exit command for safe shutdown

Separate mock_model() function

Hardcoded greeting when the input contains hello

Echo-style response for other normal input

Five-turn conversation history

history command to display stored turns

Dynamic memory using malloc() and free()

Calculator tool for +, -, *, and /

Division-by-zero handling

Automated Bash testing

Basic memory-leak testing with macOS leaks or valgrind

Project Files

harness.c - Main C program

test.sh - Automated test script

README.md - Project documentation

vibe_coding_log.md - AI-assisted development log

github.txt - Direct link to the GitHub repository

Compile

Compile the program with:

gcc harness.c -o harness

Run

Run the program with:

./harness

Program Usage

Normal Input

Normal input is sent to mock_model().

Example:

You: how are you
Assistant: how are you

Hello Response

If the input contains the word hello, the mock model returns a hardcoded greeting.

Example:

You: hello
Assistant: Hello! Nice to meet you!

The hello check is case-sensitive.

Calculator Tool

Calculator requests use this format:

calc number operator number

Example:

You: calc 10 + 5
Assistant: 15

Supported operators:

+
-
*
/

Division by zero is handled safely:

You: calc 10 / 0
Assistant: Error: cannot divide by zero.

Invalid calculator input produces an error response instead of being sent to the mock model.

Conversation History

The harness stores the five most recent conversation turns.

Each turn contains:

the user's message

the assistant response

Enter:

history

to display the currently stored turns.

The history command itself is not stored as a conversation turn.

If a sixth conversation turn is added, the oldest turn is removed and its dynamically allocated memory is freed. The remaining turns are shifted so that only the newest five remain.

Memory Management

Conversation messages are stored using dynamically allocated memory.

The program:

allocates memory for saved user and assistant strings with malloc()

frees the oldest turn when the five-turn history is full

frees all remaining stored messages before the program exits

checks for failed memory allocation before saving a new turn

Automated Testing

Run the automated tests with:

bash test.sh

The script checks:

successful compilation

hello response

calculator functionality

division-by-zero handling

five-turn conversation history

exit behavior

basic memory leaks

The history test sends six messages and verifies that the oldest message is removed while the newest five remain.

The memory test also sends more than five messages so that the history-overflow cleanup path is exercised.

On macOS, the script uses the leaks utility when available. If leaks is unavailable, it uses valgrind when installed.

A successful test run on macOS produces:

PASS: Compile
PASS: Hello response
PASS: Calculator
PASS: Division by zero
PASS: 5-turn history
PASS: Exit command
PASS: Memory leaks
Testing complete.

Final Implementation

The final project satisfies the main Project 1 requirements by providing:

a terminal-based core loop

a mock model function

bounded conversation context

safe dynamic memory management

calculator tool execution

AI-assisted automated testing

basic memory-leak verification