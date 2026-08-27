ECE 309 Project 1 - Vibe Coding Log

Om Panchal
ECE 309
Fall 2026

Project Development

For this project, I used ChatGPT as an AI coding assistant and followed a Specification Driven Development approach. I first defined the architectural rules and technical requirements for the harness before asking the AI to generate code. I then built the project one section at a time. After each major section was generated, I added it to the version I already had in VS Code, compiled the program, tested the new feature, and then continued to the next requirement.

Step 1 - Initial Specification and Architecture

Prompt:

I need to build a minimal LLM agent harness in standard C. Before generating the full program, help me define a simple architecture and development plan. The program must compile in a POSIX environment and should not use external libraries or real LLM APIs. It needs a terminal input loop, a mock model function, memory for the five most recent conversation turns, and a calculator tool for mathematical requests. I want to build each section separately and test each section before moving on.

AI Response:

The AI recommended separating the harness into a main terminal loop, a mock_model() function, conversation-history management, tool routing, a calculator tool, and memory cleanup. The main loop would repeatedly read input, check whether the user wanted to exit, determine whether the request belonged to the calculator or mock model, print the response, save the turn in history, and continue.

Testing:

There was no code to test yet. I used this architecture as the plan for the rest of the project and then requested each section separately.

Step 2 - Basic Input Loop

Prompt:

Create the first section of the harness in standard C. Use a while loop and fgets() to continuously read terminal input. Store the input in a character array, remove the newline added by fgets(), and safely end the program if the user types "exit". For now, just print the input back so I can verify the loop before adding other features.

AI Response:

#include <stdio.h>
#include <string.h>

int main(void) {
char input[256];

while (1) {
    printf("You: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        break;
    }

    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "exit") == 0) {
        printf("Goodbye!\n");
        break;
    }

    printf("Mock model received: %s\n", input);
}

return 0;

}

Testing:

I compiled the program with:

gcc harness.c -o harness

I ran it with:

./harness

I tested normal text and the exit command. The program accepted terminal input and ended correctly when I entered exit.

Step 3 - Mock Model

Prompt:

Add a separate mock_model() function to the program. The main loop should pass normal user input to this function. If the input contains the word "hello", return a hardcoded greeting. Otherwise, return a simple response containing the user's original message. Store the result in a response array so it can later be saved in conversation history.

AI Response:

The AI changed the mock model so it filled a response buffer instead of printing directly. The generated function was:

void mock_model(char input[], char response[]) {
if (strstr(input, "hello") != NULL) {
strcpy(response, "Hello! How can I help you?");
}
else {
snprintf(response, RESPONSE_SIZE, "You said: %s", input);
}
}

The main loop was also changed so normal user input was passed to mock_model() and the returned response was printed.

Testing:

I tested hello, normal text, and exit. The program returned the hardcoded greeting when hello was present and returned a simple simulated response for other normal input.

Step 4 - Five-Turn Conversation History

Prompt:

Add context management to the current harness. The program needs to safely store the five most recent conversation turns. Each turn should contain both the user's message and the assistant's response. Use a struct for one turn and use dynamic memory to store copies of the strings. When a sixth turn is added, free the oldest turn, shift the remaining turns, and save the newest turn. Add a "history" command so I can verify what is stored. Free all remaining allocated memory before the program exits.

AI Response:

The AI introduced a Turn structure and a maximum history size of five:

typedef struct {
char *user;
char *assistant;
} Turn;

#define MAX_TURNS 5

It also generated a copy_string() helper using malloc(), a save_turn() function, a show_history() function, and a free_history() function.

The string-copy helper was:

char *copy_string(const char *source) {
char *copy = malloc(strlen(source) + 1);

if (copy != NULL) {
    strcpy(copy, source);
}

return copy;

}

The cleanup function was:

void free_history(Turn history[], int count) {
for (int i = 0; i < count; i++) {
free(history[i].user);
free(history[i].assistant);
}
}

When the history was already full, save_turn() freed the oldest user and assistant strings, shifted the remaining turns left, and stored the new turn at the end.

Testing:

I entered more than five normal messages and then entered history. The program correctly removed the oldest message and displayed only the five most recent conversation turns.

Step 5 - Calculator Tool

Prompt:

Add tool execution to the current harness by creating a calculator_tool() function. Calculator requests should use the format "calc number operator number", such as "calc 10 + 5". Support addition, subtraction, multiplication, and division. Use sscanf() to parse the request, detect division by zero, detect unsupported operators, and store the result in the same response buffer used by the mock model. Calculator requests should go to the tool instead of mock_model().

AI Response:

The AI added calculator_tool(), which used sscanf() to parse two numbers and an operator. The generated function was:

int calculator_tool(char input[], char response[]) {
double num1;
double num2;
char operation;

if (sscanf(input, "calc %lf %c %lf", &num1, &operation, &num2) != 3) {
    return 0;
}

double result;

switch (operation) {
    case '+':
        result = num1 + num2;
        break;
    case '-':
        result = num1 - num2;
        break;
    case '*':
        result = num1 * num2;
        break;
    case '/':
        if (num2 == 0) {
            strcpy(response, "Error: Cannot divide by zero.");
            return 1;
        }
        result = num1 / num2;
        break;
    default:
        strcpy(response, "Error: Unsupported operator.");
        return 1;
}

snprintf(response, RESPONSE_SIZE, "Calculator result: %.2f", result);
return 1;

}

The main loop was updated so input beginning with "calc " was routed to calculator_tool() and other normal input was routed to mock_model().

Testing:

I tested addition, subtraction, multiplication, division, and division by zero. For example, calc 10 + 5 returned Calculator result: 15.00 and calc 10 / 0 returned the division-by-zero error. I also checked history afterward and confirmed that calculator requests and responses were stored as conversation turns.

Step 6 - Review the Combined Harness

Prompt:

Review the current harness architecture. The program now has a terminal input loop, an exit command, mock_model(), five-turn conversation history, dynamic memory allocation, a calculator tool, tool routing, and memory cleanup. Tell me what I should manually test before creating the automated test script.

AI Response:

The AI recommended testing normal input, the hello response, calculator input, division by zero, history with fewer than five turns, history with more than five turns, exit behavior, and memory cleanup.

Testing:

I manually tested each of those paths before moving on to automated testing. The individual sections worked together correctly in the combined harness.

Step 7 - Automated Bash Testing

Prompt:

Create a simple Bash testing script named test.sh for my C harness. The script should compile harness.c, test the hello response, test the calculator tool, test division by zero, send more than five conversation turns and verify that only the newest five remain, test the exit command, and print PASS or FAIL for each test. Keep the script simple enough for me to understand.

AI Response:

The AI generated test.sh to compile harness.c and pipe predefined input into the program using printf. It used grep to check the output and printed PASS or FAIL for each test. The script included tests for compilation, hello, calculator functionality, division by zero, five-turn history, and exit behavior.

Testing:

I ran:

bash test.sh

The functional tests produced:

PASS: Program compiled.
PASS: Hello test
PASS: Calculator test
PASS: Division by zero test
PASS: History keeps last 5 turns
PASS: Exit test

Step 8 - Memory Leak Testing

Prompt:

My conversation history uses malloc() and free(). I am using macOS. Show me a simple way to check the compiled program for basic memory leaks.

AI Response:

The AI recommended using the macOS leaks utility with this command:

printf "hello\ncalc 5 + 5\nexit\n" | leaks --atExit -- ./harness

Testing:

I ran the command and the leaks report showed:

0 leaks for 0 total leaked bytes

This confirmed that the dynamically allocated conversation memory was being released during the test.

Step 9 - Add Memory Testing to test.sh

Prompt:

Add a basic memory-leak check to test.sh so the automated script also checks memory management. If the macOS leaks tool is available, use it. If leaks is not available but valgrind is available, use valgrind. Print PASS when no leaks are detected and keep this as a separate test.

AI Response:

The AI generated this additional test:

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

Testing:

I added this section to the existing test.sh file and ran the script again. On my Mac, the final test script produced:

PASS: Program compiled.
PASS: Hello test
PASS: Calculator test
PASS: Division by zero test
PASS: History keeps last 5 turns
PASS: Exit test
PASS: Memory leak test

Testing complete.

Step 10 - Final Cleanup

Prompt:

Review the final project files and check whether the implementation matches the project requirements for standard C, POSIX compilation, the terminal core loop, mock model, five-turn context management, safe dynamic memory handling, tool execution, automated testing, memory-leak testing, README documentation, and the vibe coding log. Identify anything that should be cleaned up before submission.

AI Response:

The AI reviewed the final project and confirmed that the main implementation requirements were present. It recommended using main(void), making sure harness.c ended with a newline, updating README.md so it matched the final memory test, and keeping unnecessary generated files such as the compiled harness executable and .vscode folder out of the repository.

Testing:

I made the final cleanup changes and ran bash test.sh again. All functional tests and the memory-leak test passed.

Final Result

The completed project includes a standard C terminal harness, a mock model, five-turn conversation context, calculator tool execution, dynamic memory allocation and cleanup, automated functional testing, automated memory-leak checking, a README, a vibe-coding development log, and a GitHub submission link. I followed the SDD process by defining the architectural requirements first, requesting each major section separately, integrating each section with the existing code, and verifying the program after each major change.