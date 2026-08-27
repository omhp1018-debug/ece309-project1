// Om Panchal | harness.c | ECE309F26

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TURNS 5
#define INPUT_SIZE 256
#define RESPONSE_SIZE 512

typedef struct {
    char *user;
    char *assistant;
} Turn;


// Creates a copy of a string using dynamically allocated memory.
char *copy_string(const char *source) {
    char *copy = malloc(strlen(source) + 1);

    if (copy != NULL) {
        strcpy(copy, source);
    }

    return copy;
}


// Simulates a simple LLM.
void mock_model(char input[], char response[]) {
    if (strstr(input, "hello") != NULL) {
        strcpy(response, "Hello! How can I help you?");
    }
    else {
        snprintf(response, RESPONSE_SIZE, "You said: %s", input);
    }
}


// Calculator tool.
int calculator_tool(char input[], char response[]) {
    double num1;
    double num2;
    char operation;

    // Read input in the form:
    // calc 5 + 3
    if (sscanf(input, "calc %lf %c %lf",
               &num1, &operation, &num2) != 3) {
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

    snprintf(response, RESPONSE_SIZE,
             "Calculator result: %.2f", result);

    return 1;
}


// Saves one conversation turn.
void save_turn(Turn history[], int *count,
               char user_text[], char assistant_text[]) {

    char *user_copy = copy_string(user_text);
    char *assistant_copy = copy_string(assistant_text);

    if (user_copy == NULL || assistant_copy == NULL) {
        free(user_copy);
        free(assistant_copy);

        printf("Memory allocation failed.\n");
        return;
    }

    // Remove oldest turn if history is full.
    if (*count == MAX_TURNS) {

        free(history[0].user);
        free(history[0].assistant);

        for (int i = 1; i < MAX_TURNS; i++) {
            history[i - 1] = history[i];
        }

        *count = MAX_TURNS - 1;
    }

    history[*count].user = user_copy;
    history[*count].assistant = assistant_copy;

    (*count)++;
}


// Displays conversation history.
void show_history(Turn history[], int count) {

    printf("\n--- Conversation History ---\n");

    if (count == 0) {
        printf("No conversation history.\n");
    }

    for (int i = 0; i < count; i++) {
        printf("You: %s\n", history[i].user);
        printf("Assistant: %s\n", history[i].assistant);
    }

    printf("----------------------------\n\n");
}


// Frees all allocated memory.
void free_history(Turn history[], int count) {

    for (int i = 0; i < count; i++) {
        free(history[i].user);
        free(history[i].assistant);
    }
}


int main(void) {

    char input[INPUT_SIZE];
    char response[RESPONSE_SIZE];

    Turn history[MAX_TURNS] = {0};
    int history_count = 0;

    while (1) {

        printf("You: ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove newline.
        input[strcspn(input, "\n")] = '\0';

        // Exit command.
        if (strcmp(input, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        // History command.
        if (strcmp(input, "history") == 0) {
            show_history(history, history_count);
            continue;
        }

        // Check whether the calculator tool should handle input.
        if (strncmp(input, "calc ", 5) == 0) {

            if (!calculator_tool(input, response)) {
                strcpy(response,
                       "Invalid calculator format. Example: calc 5 + 3");
            }
        }

        // Otherwise send input to mock model.
        else {
            mock_model(input, response);
        }

        printf("Assistant: %s\n", response);

        // Save turn in context history.
        save_turn(history, &history_count, input, response);
    }

    free_history(history, history_count);

    return 0;
}