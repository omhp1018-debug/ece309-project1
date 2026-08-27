ECE 309 Project 1 - Vibe Coding Log

Om Panchal
ECE 309
Fall 2026

Project Development

For this project, I used an AI assistant to help generate and improve the C program. I worked on the project in smaller stages instead of asking the AI to create the entire program at once. I started with the basic input loop and then added the mock model, conversation history, calculator tool, automated testing, and memory testing.

Step 1 - Initial Program Requirements

Prompt:

I need to build Project 1 for ECE 309: a minimal LLM agent harness in standard C. Use standard C and no external libraries. Create a terminal-based loop using fgets() to read user input. If the user enters "exit", safely terminate the program. Pass normal user input to a mock_model() function that simulates an LLM. If the input contains "hello", the mock model should return a hardcoded greeting. Otherwise, the mock model should echo a simple response. Organize the code so we can later add storage for the last 5 conversation turns and a calculator tool. Use safe fixed-size character arrays and add comments explaining the important parts of the code. First, explain the architecture you recommend and do not write the full program yet.

AI Response:

The AI recommended developing the program in stages. The main parts included a terminal input loop, exit command, mock model, conversation history, calculator tool, memory cleanup, and automated testing. The main loop would continuously take user input, decide how the input should be handled, generate a response, save the conversation, and repeat until the user entered "exit".

Step 2 - Basic Input Loop

Prompt:

Create the first stage of the program. Write a simple standard C program that uses fgets() to continuously read user input. Remove the newline from the input and terminate safely when the user enters "exit". For now, simply print the input back as a mock response.

AI Response:

The AI generated the first version of harness.c using stdio.h and string.h. The program created a 256-character input array and used a while loop to continuously ask the user for input. It used fgets() to read a full line from the terminal, strcspn() to remove the newline character, and strcmp() to check whether the user entered "exit". If exit was entered, the program printed "Goodbye!" and ended.

I compiled the program using:

gcc harness.c -o harness

I ran the program using:

./harness

The program successfully accepted user input and ended when I entered exit.

Step 3 - Mock Model

Prompt:

Move the simulated LLM behavior into a separate mock_model() function. If the user's input contains the word "hello", return a hardcoded greeting. Otherwise, echo the user's message as the simulated response. Keep the code simple and use only standard C libraries.

AI Response:

The AI added a separate mock_model() function. The function uses strstr() to check whether the user's input contains the word "hello". If hello is detected, the program responds with "Assistant: Hello! How can I help you?" If hello is not detected, the mock model returns a simple response containing the user's original message. I compiled and tested the program again after adding this feature.

Step 4 - Conversation History

Prompt:

Extend the program so it stores the five most recent conversation turns. Each turn should contain both the user's message and the assistant's response. Use dynamic memory safely. When more than five turns exist, free the oldest turn and remove it from the history. Add a "history" command that displays the currently stored turns. Free all remaining allocated memory before the program exits.

AI Response:

The AI added a Turn structure containing char *user and char *assistant. The program stores a maximum of five Turn structures. A copy_string() function was added to use malloc() to create copies of each message, and a save_turn() function was added to store conversations. When five turns are already stored, the oldest user message and assistant response are freed, the remaining turns are shifted forward, and the newest turn is stored. The AI also added show_history() to display the saved conversation and free_history() to release remaining allocated memory before the program exits.

I tested the program by entering more than five messages and then entering "history". The program correctly displayed only the five most recent conversation turns.

Step 5 - Calculator Tool

Prompt:

Add a calculator tool to the harness. If the user's input begins with "calc ", the harness should route the request to a calculator function instead of the mock model. Support addition, subtraction, multiplication, and division. Use a format such as "calc 10 + 5". Detect division by zero and invalid operators.

AI Response:

The AI added a calculator_tool() function that uses sscanf() to read two numbers and an operator from the user's input. The calculator supports addition, subtraction, multiplication, and division. A switch statement is used to determine which mathematical operation should be performed. For example, "calc 10 + 5" produces "Assistant: Calculator result: 15.00". The program also checks whether the second number is zero before division and returns an error if necessary. The main input loop checks whether the input begins with "calc ". Calculator requests are sent to calculator_tool(), while other requests are sent to mock_model().

Step 6 - Automated Testing

Prompt:

I have a C program called harness.c that implements a simple LLM harness. Please create a simple Bash testing script named test.sh. The script should compile harness.c, test the hello response, test the calculator tool, test that conversation history keeps only the last 5 turns, test that the exit command ends the program, print PASS or FAIL for each test, and remain simple enough for a beginner to understand.

AI Response:

The AI created a Bash testing script named test.sh. The script compiles harness.c and automatically sends different inputs into the program. It checks the program's output using grep. The tests include program compilation, the hello response, calculator functionality, division by zero, five-turn conversation history, and the exit command.

I ran:

bash test.sh

The results were:

Compiling harness...
PASS: Program compiled.
PASS: Hello test
PASS: Calculator test
PASS: Division by zero test
PASS: History keeps last 5 turns
PASS: Exit test
Testing complete.

Step 7 - Memory Testing

Prompt:

How can I perform a basic memory leak test on this C program on macOS?

AI Response:

The AI recommended using the macOS leaks tool. I first checked that the command was available using "which leaks", which returned "/usr/bin/leaks". I compiled the program again using "gcc harness.c -o harness" and then ran:

printf "hello\ncalc 5 + 5\nexit\n" | leaks --atExit -- ./harness

The leaks tool reported:

0 leaks for 0 total leaked bytes

This showed that the dynamically allocated conversation memory was successfully freed during the test.

Final Result

The completed program contains a terminal input loop, exit command, mock LLM function, five-turn conversation history, dynamic memory allocation and cleanup, calculator tool, division by zero handling, automated testing, and memory leak testing. Building the project one feature at a time made it easier to test each part before moving on to the next requirement.
