ECE 309 Project 1 - Vibe Coding Log

Om Panchal
ECE 309
Fall 2026

Project
ECE 309 Project 1: An LLM Mini-Harness in C via Vibe Coding

AI Assistant
ChatGPT

Purpose of This Log
This file documents the prompts, iterations, AI responses, debugging steps, and testing workflow used to create harness.c and test.sh.

The overall architectural rules used during development were:

Use standard C only.

Keep the program beginner-friendly.

Use an infinite input loop with fgets().

Use exit to safely terminate the program.

Send normal user input to a separate mock_model() function.

Store only the most recent 5 conversation turns.

Use dynamic memory with malloc() and free().

Provide a history command that does not save itself.

Route calc requests to a calculator tool instead of mock_model().

Support +, -, *, and /.

Handle division by zero.

Create a separate Bash test script.

Automatically test state management and basic memory leaks.

Interaction 1 - Initial Specification / Core Loop

User Prompt

I need to write a simple command-line program in C. I am a beginner, so please keep the
code as simple as possible. Do not use external libraries, only standard ones like <stdio.h>
and <string.h>. Here is the specification for the program:

It should run an infinite while loop that asks for user input using fgets.

If the user types 'exit', the loop should break and the program should end.

If the user types a sentence containing the word 'hello', the program should print a
hardcoded greeting.

If the user types anything else, it should echo their input back to them.

Please add clear, line-by-line comments explaining what the code is doing.

AI Response

Here is a simple beginner-friendly version using only <stdio.h> and <string.h>:

#include <stdio.h>      // Allows us to use printf() and fgets()
#include <string.h>     // Allows us to use strcmp(), strstr(), and strcspn()

int main() {

// Create an array to store the user's input.
char input[256];

// Keep running the program until the user types "exit".
while (1) {

    // Ask the user to enter something.
    printf("Enter something: ");

    // Read a line of input from the user.
    fgets(input, sizeof(input), stdin);

    // Remove the newline character that fgets() adds.
    input[strcspn(input, "\n")] = '\0';

    // Check if the user typed exactly "exit".
    if (strcmp(input, "exit") == 0) {

        // Leave the while loop.
        break;
    }

    // Check if the user's input contains the word "hello".
    else if (strstr(input, "hello") != NULL) {

        // Print a hardcoded greeting.
        printf("Hello! Nice to meet you!\n");
    }

    // If the input was not "exit" and did not contain "hello".
    else {

        // Print the user's input back to them.
        printf("You said: %s\n", input);
    }
}

// Print a message before the program ends.
printf("Goodbye!\n");

// End the program successfully.
return 0;

}

Example:

Enter something: how are you?
You said: how are you?

Enter something: hello there
Hello! Nice to meet you!

Enter something: exit
Goodbye!

This version treats hello as case-sensitive, so hello works but Hello does not.

Development Result

The first version established the basic fgets() loop, exit command, hello behavior, and echo behavior.

Interaction 2 - Moving Model Behavior Into mock_model()

User Prompt

Now can you change the program so the hello and echo behavior are inside a separate mock_model() function? I want the main loop to send normal user input to that function and then print the response. Keep the code simple and explain what you changed.

AI Response

Sure. The main change is that main() now only handles getting input, checking for "exit", and printing the response. The "hello" check and echo behavior are moved into a separate mock_model() function.

#include <stdio.h>      // Allows us to use printf() and fgets()
#include <string.h>     // Allows us to use strcmp(), strstr(), strcpy(), and strcspn()

// This function decides what response to give based on the user's input.
void mock_model(char input[], char response[]) {

// Check if the input contains the word "hello".
if (strstr(input, "hello") != NULL) {

    // Store a hardcoded greeting in the response array.
    strcpy(response, "Hello! Nice to meet you!");
}

// If the input does not contain "hello".
else {

    // Copy the user's input into the response array.
    strcpy(response, input);
}

}

int main() {

// Store the user's input.
char input[256];

// Store the response from mock_model().
char response[256];

// Keep running until the user types "exit".
while (1) {

    // Ask the user for input.
    printf("Enter something: ");

    // Read the user's input.
    fgets(input, sizeof(input), stdin);

    // Remove the newline character added by fgets().
    input[strcspn(input, "\n")] = '\0';

    // Check if the user typed exactly "exit".
    if (strcmp(input, "exit") == 0) {

        // Leave the loop.
        break;
    }

    // Send normal user input to mock_model().
    mock_model(input, response);

    // Print the response returned by mock_model().
    printf("%s\n", response);
}

// Print a message before ending the program.
printf("Goodbye!\n");

// End the program successfully.
return 0;

}

What changed

Previously, main() handled all three behaviors: checking for "exit", checking for "hello", and echoing input.

Now the responsibilities are split:

main() gets input and handles "exit".

mock_model() checks for "hello" and creates the response.

main() prints whatever mock_model() puts into response.

For example:

Enter something: hello there
Hello! Nice to meet you!

Enter something: how are you?
how are you?

Enter something: exit
Goodbye!

The important new line is:

mock_model(input, response);

You can think of it as: send input to the mock model, and let it fill in response.

Development Result

The mock model logic was separated from the main loop so normal messages could be routed through a model-like function.

Interaction 3 - Adding Five-Turn Conversation History

User Prompt

Now I need to add conversation history. Can you make the program store the last 5 conversation turns, where each turn has both the user message and the assistant response? Use a struct and dynamic memory with malloc and free, but keep it beginner friendly and explain what each new part does.

AI Response

Yes. A simple beginner-friendly way is to make a ConversationTurn struct containing two dynamically allocated strings: one for the user message and one for the assistant response.

When there are already 5 turns, the program frees the oldest turn, shifts the other four forward, and stores the newest turn at the end.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>     // Allows us to use malloc() and free()

// The maximum number of conversation turns we want to remember.
#define MAX_HISTORY 5

// A struct groups the user message and assistant response together.
struct ConversationTurn {
char *user_message;
char *assistant_response;
};

// This function creates the assistant's response.
void mock_model(char input[], char response[]) {

// If the input contains "hello", return a greeting.
if (strstr(input, "hello") != NULL) {
    strcpy(response, "Hello! Nice to meet you!");
}

// Otherwise, echo the user's input.
else {
    strcpy(response, input);
}

}

// This function saves one conversation turn into history.
void add_to_history(struct ConversationTurn history[],
int *history_count,
char user_message[],
char assistant_response[]) {

int i;

// If we already have 5 turns, remove the oldest one.
if (*history_count == MAX_HISTORY) {

    // Free the memory used by the oldest user message.
    free(history[0].user_message);

    // Free the memory used by the oldest assistant response.
    free(history[0].assistant_response);

    // Move turns 1-4 one position to the left.
    for (i = 0; i < MAX_HISTORY - 1; i++) {
        history[i] = history[i + 1];
    }

    // We now have room for one new turn.
    *history_count = MAX_HISTORY - 1;
}

// Allocate enough memory for the new user message.
history[*history_count].user_message =
    malloc(strlen(user_message) + 1);

// Allocate enough memory for the new assistant response.
history[*history_count].assistant_response =
    malloc(strlen(assistant_response) + 1);

// Check that malloc worked.
if (history[*history_count].user_message == NULL ||
    history[*history_count].assistant_response == NULL) {

    printf("Memory allocation failed.\n");
    return;
}

// Copy the user message into the newly allocated memory.
strcpy(history[*history_count].user_message, user_message);

// Copy the assistant response into the newly allocated memory.
strcpy(history[*history_count].assistant_response,
       assistant_response);

// Increase the number of stored turns.
(*history_count)++;

}

int main() {

// Store the current user input.
char input[256];

// Store the current assistant response.
char response[256];

// Array that can hold up to 5 conversation turns.
struct ConversationTurn history[MAX_HISTORY];

// Keep track of how many turns are currently stored.
int history_count = 0;

// Used later for loops.
int i;

// Keep running until the user types "exit".
while (1) {

    // Ask the user for input.
    printf("You: ");

    // Read the user's input.
    fgets(input, sizeof(input), stdin);

    // Remove the newline character added by fgets().
    input[strcspn(input, "\n")] = '\0';

    // End the program if the user types "exit".
    if (strcmp(input, "exit") == 0) {
        break;
    }

    // Send the input to our mock model.
    mock_model(input, response);

    // Print the assistant's response.
    printf("Assistant: %s\n", response);

    // Save both messages as one conversation turn.
    add_to_history(history, &history_count, input, response);
}

// Display the conversation history before exiting.
printf("\nLast %d conversation turns:\n", history_count);

for (i = 0; i < history_count; i++) {
    printf("\nTurn %d\n", i + 1);
    printf("You: %s\n", history[i].user_message);
    printf("Assistant: %s\n", history[i].assistant_response);
}

// Free all dynamically allocated memory before the program ends.
for (i = 0; i < history_count; i++) {
    free(history[i].user_message);
    free(history[i].assistant_response);
}

printf("\nGoodbye!\n");

return 0;

}

The new struct

This is the main addition:

struct ConversationTurn {
char *user_message;
char *assistant_response;
};

A struct lets us group related information together. Each conversation turn contains a user message and an assistant response.

The history array

Inside main() we create:

struct ConversationTurn history[MAX_HISTORY];

Since MAX_HISTORY is 5, the array has space for five conversation turns.

Why we use malloc()

Instead of giving every message a fixed-size array, each struct contains pointers. We allocate enough memory for each string using:

malloc(strlen(user_message) + 1);

The extra byte is for the terminating \0.

Why history_count uses a pointer

The function receives an int *history_count, and main() passes &history_count, so add_to_history() can update the real count.

What happens after 5 messages?

When the history is full, the oldest strings are freed, the remaining turns shift left, and the newest turn is stored at the end.

Why free() is important

Any memory created with malloc() should eventually be released using free().

Development Result

The program now stores the latest five user/assistant pairs using a struct and dynamic memory.

Interaction 4 - Adding the history Command

User Prompt

Can you add a history command now so when I type history, the program prints the conversation turns that are currently stored? I only want it to show the last 5 turns, and I do not want the word history itself saved as a conversation turn. Keep the code simple and explain what you added.

AI Response

Yes. The simplest way is to check for "history" in main() before sending the input to mock_model() or saving it.

The main new logic is:

if (strcmp(input, "history") == 0) {
print_history(history, history_count);
continue;
}

The continue; is especially important. It tells the program to immediately go back to the top of the while loop, so "history" never reaches mock_model() and never reaches add_to_history().

A separate print_history() function loops through the currently stored conversation turns and prints them.

Example behavior:

You: hello
Assistant: Hello! Nice to meet you!

You: how are you
Assistant: how are you

You: history

Conversation History:

Turn 1
You: hello
Assistant: Hello! Nice to meet you!

Turn 2
You: how are you
Assistant: how are you

Typing history multiple times does not cause the word history itself to appear in the stored conversation history.

Development Result

The program gained a special command for displaying state without changing that state.

Interaction 5 - Reviewing Dynamic Memory Cleanup

User Prompt

Since the conversation history uses malloc, can you make sure the program frees memory correctly? If the history already has 5 turns and a new one is added, free the oldest turn before replacing it. Also free all remaining conversation memory before the program exits. Keep it simple and explain where and why free() is used.

AI Response

Yes. Your program should use free() in two places: when the 5-turn history is full and the oldest turn is removed, and when the program exits.

The first important location is:

free(history[0].user_message);
free(history[0].assistant_response);

These calls release the dynamically allocated strings belonging to the oldest turn before the history entries are shifted.

The second important location is at program shutdown:

for (i = 0; i < history_count; i++) {
free(history[i].user_message);
free(history[i].assistant_response);
}

This releases all strings that remain in the history when the program ends.

A useful rule is:

malloc() = reserve memory
free()   = release that memory

The revised version also uses temporary pointers when allocating the new strings. If either malloc() fails, any allocation that did succeed is freed before returning.

Development Result

The memory ownership rules were clarified and cleanup was verified for both history rollover and program termination.

Interaction 6 - Adding Calculator Tool Execution

User Prompt

Now I need to add the tool execution part of the project. Can you add a simple calculator command where I type something like calc 10 + 5? Support +, -, *, and /. Calculator requests should go to the calculator instead of mock_model(), and it should handle division by zero. Keep the code simple and explain what you added.

AI Response

Yes. We can add a separate calculator() function. The main loop checks whether the input starts with "calc ". If it does, it sends the expression to the calculator instead of mock_model().

The calculator reads input with:

sscanf(input + 5, "%lf %c %lf",
&number1, &operator, &number2);

The input + 5 skips the characters in "calc ".

The routing logic in main() is:

if (strncmp(input, "calc ", 5) == 0) {
calculator(input, response);
}
else {
mock_model(input, response);
}

The calculator supports the four operators:







/

For division, it checks whether the second number is zero before performing the calculation:

if (number2 == 0) {
strcpy(response, "Error: cannot divide by zero.");
}

Example behavior:

You: calc 10 + 5
Assistant: 15

You: calc 20 - 8
Assistant: 12

You: calc 6 * 4
Assistant: 24

You: calc 10 / 2
Assistant: 5

You: calc 10 / 0
Assistant: Error: cannot divide by zero.

Calculator requests are stored as normal conversation turns, but the calculator is used instead of mock_model() to produce their responses.

Development Result

The tool-execution requirement was implemented by routing calc requests to a dedicated calculator function.

Interaction 7 - Full harness.c Review

User Prompt

Can you review the full harness.c now and make sure everything works together correctly? It should have the input loop, mock_model(), last 5 conversation turns, history command, dynamic memory cleanup, calculator tool, and exit. Tell me what I should manually test before I make the automated test script.

AI Response

The full program was reviewed for:

the input loop,

normal mock_model() routing,

calculator routing,

history,

the five-turn history limit,

malloc() / free() behavior,

and exit.

The recommended manual tests were:

hello

normal echo text

calculator addition

calculator subtraction

calculator multiplication

calculator division

division by zero

unsupported calculator operator

malformed calculator input

empty history

normal history storage

verifying history is not saved

sending six messages and verifying only messages 2 through 6 remain

verifying calculator requests are stored

exit

verifying exit is not saved

The review also confirmed that the oldest user and assistant strings are freed before a full history is shifted and that all remaining conversation memory is freed after the loop.

Development Result

The completed harness architecture was manually reviewed before automated testing was created.

Interaction 8 - Creating test.sh

User Prompt

Now create a separate Bash testing script named test.sh for my harness. It should compile harness.c and automatically test the hello response, calculator, division by zero, the 5-turn conversation history, and the exit command. For the history test, send 6 messages and verify that only the newest 5 remain. Print PASS or FAIL for each test and keep the script simple enough for me to understand.

AI Response

A separate test.sh file was created.

The script:

compiles harness.c,

tests the hello response,

tests the calculator,

tests division by zero,

sends six messages and checks that only the newest five remain,

tests the exit command,

and prints PASS or FAIL for each test.

The history test stores the output from:

printf "message1\nmessage2\nmessage3\nmessage4\nmessage5\nmessage6\nhistory\nexit\n" | ./harness

It then verifies that message1 is absent from the displayed history and message2 through message6 are present.

The script can be run with:

bash test.sh

Development Result

Automated deterministic functional testing was added in a separate Bash script.

Interaction 9 - Adding Basic Memory-Leak Testing

User Prompt

Add one more test to test.sh for basic memory leaks. I am using macOS, so use the leaks tool if it is available, and use valgrind as a fallback if it is installed. During the memory test, send more than 5 conversation messages so it also tests freeing the oldest history entries. Print PASS, FAIL, or SKIP for the memory test and keep the code simple.

AI Response

The testing script was updated with a sixth test.

The memory test sends more than five conversation messages:

memory_input="one
two
three
four
five
six
exit
"

It first checks whether the macOS leaks tool is available:

if command -v leaks >/dev/null 2>&1; then

If available, it runs:

printf "%s" "$memory_input" | leaks --atExit -- ./harness

and checks for:

0 leaks for 0 total leaked bytes

If leaks is unavailable, the script checks for valgrind and looks for:

definitely lost: 0 bytes

If neither tool is available, it prints:

SKIP: Memory leaks (leaks/valgrind not installed)

Development Result

The automated test suite now checks both functional behavior and basic dynamic-memory cleanup.

Interaction 10 - First Automated-Test Debugging Iteration

User Input

The user provided a screenshot after running:

bash test.sh

The screenshot showed:

PASS: Compile
FAIL: Hello response
FAIL: Calculator
FAIL: Division by zero
FAIL: 5-turn history
PASS: Exit command
Testing complete.

AI Response

The AI identified that test.sh was running, because compilation and exit passed, but the expected strings in the script did not match the current output of harness.c.

The AI recommended checking the actual output of:

hello
calc 10 + 5
calc 10 / 0
message1
message2
message3
message4
message5
message6
history
exit

It also noticed that the displayed copy of test.sh did not yet contain the memory test.

Development Result

The failure was treated as a test-expectation mismatch rather than immediately changing the program.

Interaction 11 - Uploading Current harness.c and test.sh for Comparison

User Input

The user uploaded the current harness.c and test.sh.

AI Response

The AI compared the two files and found the exact mismatch.

The current harness produced:

Hello! Nice to meet you!
Assistant: 15
Error: cannot divide by zero.
Conversation History:

but the older test script was checking for different strings, including:

Hello! How can I help you?
Calculator result: 15.00
Error: Cannot divide by zero.
--- Conversation History ---

The AI updated the test script to match the actual harness output and included the memory test.

The corrected script was tested against the uploaded harness and produced:

PASS: Compile
PASS: Hello response
PASS: Calculator
PASS: Division by zero
PASS: 5-turn history
PASS: Exit command
SKIP: Memory leaks (leaks/valgrind not installed)
Testing complete.

The SKIP occurred only because the AI's execution environment did not contain leaks or valgrind.

Development Result

The automated test suite was aligned with the final program's exact output format.

Interaction 12 - Corrected test.sh Still Not Replaced Locally

User Prompt / Input

The user provided a screenshot showing that the local test.sh still ended near line 78 and produced the older failures.

AI Response

The AI determined that the corrected test.sh had not replaced the older local file yet.

It explained that the corrected script should contain checks for:

Hello! Nice to meet you!
Assistant: 15
Error: cannot divide by zero.
Conversation History:

and should also include the memory test.

The AI instructed the user to replace the old test.sh, save it, and run:

bash test.sh

again.

Development Result

The issue was identified as a local-file version mismatch rather than a C-code problem.

Interaction 13 - Final Test Run

User Input

The user provided a screenshot of the corrected script and final terminal output.

The terminal showed:

PASS: Compile
PASS: Hello response
PASS: Calculator
PASS: Division by zero
PASS: 5-turn history
PASS: Exit command
PASS: Memory leaks
Testing complete.

AI Response

The AI confirmed that the final result was correct and that all functional tests and the memory-leak test passed.

It also noted that the memory test used more than five messages, so the passing result exercised the code path that frees the oldest conversation-history entries.

Development Result

Final verification passed successfully.

Final Architecture Summary

The final harness.c contains the following major components:

struct ConversationTurn

Stores one conversation turn with:

a dynamically allocated user message,

a dynamically allocated assistant response.

mock_model()

Handles normal conversation:

returns a hardcoded greeting when the input contains hello,

otherwise echoes the user's message.

calculator()

Handles tool execution for inputs beginning with:

calc

Supported operations:

addition,

subtraction,

multiplication,

division.

It also handles division by zero.

Conversation History

The program stores at most five turns.

When a sixth turn arrives:

memory for the oldest user message is freed,

memory for the oldest assistant response is freed,

the remaining four turns shift left,

the new turn is stored at the end.

history Command

The history command prints the currently stored turns.

Because continue is used immediately afterward, the command itself is not passed to the mock model or stored in history.

exit Command

Typing:

exit

breaks the input loop.

All remaining dynamically allocated history strings are freed before the program terminates.

Final Automated Testing Summary

The final test.sh automatically verifies:

successful compilation,

hello response,

calculator operation,

division-by-zero handling,

five-turn state management,

exit behavior,

basic memory-leak behavior.

The history test sends six messages and verifies that only messages 2 through 6 remain.

The memory test also sends six messages so that the history-overflow cleanup code executes.

On macOS, leaks is preferred. If it is unavailable, valgrind is used as a fallback. If neither tool exists, the memory test reports SKIP.

Final Verification Output

PASS: Compile
PASS: Hello response
PASS: Calculator
PASS: Division by zero
PASS: 5-turn history
PASS: Exit command
PASS: Memory leaks
Testing complete.

This final output confirms that the harness compiled, behaved correctly, maintained its context boundary, executed the calculator tool, exited safely, and did not report basic memory leaks during the tested path.