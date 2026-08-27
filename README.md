ECE 309 Project 1 - LLM Mini-Harness in C

Om Panchal
ECE 309
Fall 2026

Project Description

For this project, I created a simple LLM agent harness using C. The program takes input from the user through the terminal and decides whether the input should be handled by a mock language model or by a calculator tool. The program also stores the five most recent conversation turns, with each turn containing the user's input and the response produced by the program. The project does not use an actual LLM API or any external libraries.

Program Features

The program reads user input using fgets(), uses a mock_model() function to simulate an LLM, gives a hardcoded response when the input contains "hello", and echoes other normal input. It also includes a calculator tool that supports addition, subtraction, multiplication, and division. The program detects division by zero, stores the five most recent conversation turns, includes a history command, uses dynamic memory allocation, frees allocated memory before exiting, and includes an automated Bash testing script.

Project Files

harness.c is the main C program and contains the input loop, mock model, calculator tool, conversation history, and memory management. test.sh is the automated testing script that compiles and tests the program. README.md explains the project and how to compile and run it. vibe_coding_log.md documents the AI-assisted development process used while creating the project.

Compiling and Running

The program can be compiled using:

gcc harness.c -o harness

After compiling, it can be run using:

./harness

Using the Program

The program continues asking for input until the user enters "exit". Entering "history" displays the saved conversation history. Calculator commands begin with "calc" followed by a number, operator, and another number. For example:

calc 10 + 5

The program responds with:

Assistant: Calculator result: 15.00

The calculator supports +, -, *, and /. The program also checks for division by zero and displays an error message instead of performing the calculation.

Mock Model

Normal input is sent to the mock_model() function. If the input contains the word "hello", the program responds with "Assistant: Hello! How can I help you?" For other normal input, the program returns a simple response containing the user's message.

Conversation History

The program stores the five most recent conversation turns. Each turn contains both the user's input and the assistant's response. When the history already contains five turns and another turn is added, the oldest turn is removed and its allocated memory is freed. Entering "history" displays the conversation turns currently stored.

Memory Management

Dynamic memory is used to store copies of the user messages and assistant responses. Memory is allocated using malloc() and released using free(). When an old conversation is removed from the history, its memory is freed. Before the program exits, all remaining conversation memory is also freed. I tested the program using the macOS leaks tool, which reported "0 leaks for 0 total leaked bytes."

Automated Testing

The automated tests can be run using:

bash test.sh

The testing script checks program compilation, the hello response, calculator functionality, division by zero, the five-turn conversation history, the exit command, and basic memory leaks. On macOS it uses the leaks tool, and if valgrind is available it can use that instead. The completed tests produced the following results:

PASS: Program compiled.
PASS: Hello test
PASS: Calculator test
PASS: Division by zero test
PASS: History keeps last 5 turns
PASS: Exit test
PASS: Memory leak test

All automated tests passed successfully.