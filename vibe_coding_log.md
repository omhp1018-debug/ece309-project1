ECE 309 Project 1 - Vibe Coding Log

Om Panchal
ECE 309
Fall 2026

Project Development

For this project, I used ChatGPT as the AI assistant and developed the harness one stage at a time instead of asking for one large final program. The prompts below are copied from the development process. For the coding stages, the AI-generated code shown is the code produced during that stage and then used as the basis for the next iteration. I compiled and tested after each major change before continuing.

Step 1 - Initial Specification and Architecture

Prompt:

I need to build Project 1 for ECE 309: a minimal LLM agent harness in standard C. I am still learning C, so keep the implementation simple and readable. Requirements: 1. Use standard C and no external libraries. 2. Create a terminal-based loop using fgets() to read user input. 3. If the user enters "exit", safely terminate the program. 4. Pass normal user input to a mock_model() function that simulates an LLM. 5. If the input contains "hello", the mock model should return a hardcoded greeting. 6. Otherwise, the mock model should return or echo a simple response. 7. Organize the code so we can later add storage for the last 5 conversation turns and a calculator/tool execution feature. 8. Use safe fixed-size character arrays and avoid unnecessary advanced C concepts. 9. Add comments explaining the important parts of the code. First, explain the architecture you recommend. Do not write the full program yet.

AI Response:

The AI recommended building the program in stages: a basic terminal input/output loop, a mock_model() function, five-turn conversation history, calculator/tool execution, memory cleanup, automated testing, documentation, and final GitHub submission. The main state machine would repeatedly read input, check for exit, route tool requests to a tool, route normal requests to the mock model, print the response, save the turn, and repeat.

Step 2 - Basic Input Loop

Prompt:

Create the first stage of the program. Write a simple standard C program that uses fgets() to continuously read user input. Remove the newline from the input and terminate safely when the user enters "exit". For now, simply print the input back as a mock response.

AI Response:

#include <stdio.h>
#include <string.h>

int main() {
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

The program accepted terminal input and ended correctly when I entered exit.

Step 3 - Mock Model

Prompt:

Move the simulated LLM behavior into a separate mock_model() function. If the user's input contains the word "hello", return a hardcoded greeting. Otherwise, echo the user's message as the simulated response. Keep the code simple and use only standard C libraries.

AI Response:

#include <stdio.h>
#include <string.h>

void mock_model(char input[]) {
if (strstr(input, "hello") != NULL) {
printf("Assistant: Hello! How can I help you?\n");
}
else {
printf("Assistant: You said: %s\n", input);
}
}

int main() {
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

    mock_model(input);
}

return 0;

}

Testing:

I tested hello, normal text, and exit. The program routed normal input through mock_model() and produced the expected hardcoded greeting for hello.

Step 4 - Five-Turn Conversation History

Prompt:

Extend the program so it stores the five most recent conversation turns. Each turn should contain both the user's message and the assistant's response. Use dynamic memory safely. When more than five turns exist, free the oldest turn and remove it from the history. Add a "history" command that displays the currently stored turns. Free all remaining allocated memory before the program exits.

AI Response:

The AI introduced a Turn structure containing char *user and char *assistant, a MAX_TURNS value of 5, a copy_string() helper using malloc(), save_turn() to store turns, show_history() to display them, and free_history() to release allocated memory. When the history was full, the oldest turn was freed, the remaining turns were shifted left, and the newest turn was added.

The generated Turn structure was:

typedef struct {
char *user;
char *assistant;
} Turn;

The generated memory-copy helper was:

char *copy_string(const char *source) {
char *copy = malloc(strlen(source) + 1);

if (copy != NULL) {
    strcpy(copy, source);
}

return copy;

}

The generated cleanup function was:

void free_history(Turn history[], int count) {
for (int i = 0; i < count; i++) {
free(history[i].user);
free(history[i].assistant);
}
}

Testing:

I entered more than five normal messages and then entered history. The oldest message was removed and only the five most recent conversation turns were displayed.

Step 5 - Calculator Tool

Prompt:

Add a calculator tool to the harness. If the user's input begins with "calc ", the harness should route the request to a calculator function instead of the mock model. Support addition, subtraction, multiplication, and division. Use a format such as "calc 10 + 5". Detect division by zero and invalid operators.

AI Response:

The AI added calculator_tool(), which uses sscanf() to parse two numbers and an operator. The generated function was:

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

The main loop was also changed so input beginning with "calc " is sent to calculator_tool() and other normal input is sent to mock_model().

Testing:

I tested addition, subtraction, multiplication, division, and division by zero. For example, calc 10 + 5 returned Calculator result: 15.00 and calc 10 / 0 returned the division-by-zero error.

Step 6 - Automated Bash Testing

Prompt:

I have a C program called harness.c that implements a simple LLM harness. Please create a simple Bash testing script named test.sh. The script should: 1. Compile harness.c. 2. Test the hello response. 3. Test the calculator tool. 4. Test that conversation history keeps only the last 5 turns. 5. Test that the exit command ends the program. 6. Print PASS or FAIL for each test. 7. Keep the script simple enough for a beginner to understand.

AI Response:

The AI generated test.sh to compile harness.c and pipe predefined input into the program. It used grep to check expected output and printed PASS or FAIL for each case. Tests were created for the hello response, calculator, division by zero, five-turn history, and exit behavior.

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

Step 7 - Manual Memory Leak Testing

Prompt:

How can I perform a basic memory leak test on this C program on macOS?

AI Response:

The AI recommended using the macOS leaks tool. The command used was:

printf "hello\ncalc 5 + 5\nexit\n" | leaks --atExit -- ./harness

Testing:

The leaks report showed:

0 leaks for 0 total leaked bytes

Step 8 - Add Memory Leak Testing to test.sh

Prompt:

How do I add the memory leak test to the automated test.sh script so the script itself checks for basic memory leaks?

AI Response:

The AI generated this additional test:

-------------------------

Test 6: Memory leaks

-------------------------

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

On my Mac, the final test script produced PASS for the memory leak test in addition to the functional tests.

Step 9 - Final Cleanup

Prompt:

Check all files to make sure I completed the assignment correctly and identify anything that should be fixed before submission.

AI Response:

The AI reviewed harness.c, test.sh, README.md, vibe_coding_log.md, and github.txt against the project specification. It recommended changing main() to main(void), ensuring a final newline in harness.c, updating README.md to mention the automated memory leak test, keeping the exact development prompts/code in the vibe coding log, and excluding the compiled harness executable and .vscode folder from the repository.

Final Result

The completed project includes a standard C terminal harness, a mock model, five-turn conversation context, calculator tool execution, dynamic memory allocation and cleanup, automated functional testing, automated memory-leak checking, a README, a vibe-coding development log, and a GitHub submission link. The project was built and tested incrementally so each requirement could be verified before moving to the next stage.