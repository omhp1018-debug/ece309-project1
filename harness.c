// Om Panchal | harness.c | ECE309F26

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_HISTORY 5

// One conversation turn contains a user message
// and an assistant response.
struct ConversationTurn {
    char *user_message;
    char *assistant_response;
};

// Handles normal conversation.
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

// Handles calculator requests.
void calculator(char input[], char response[]) {

    double number1;
    double number2;
    char operator;

    // Read two numbers and an operator from the input.
    // input + 5 skips over the word "calc ".
    if (sscanf(input + 5, "%lf %c %lf",
               &number1, &operator, &number2) != 3) {

        strcpy(response, "Invalid calculator expression.");
        return;
    }

    // Addition.
    if (operator == '+') {
        sprintf(response, "%g", number1 + number2);
    }

    // Subtraction.
    else if (operator == '-') {
        sprintf(response, "%g", number1 - number2);
    }

    // Multiplication.
    else if (operator == '*') {
        sprintf(response, "%g", number1 * number2);
    }

    // Division.
    else if (operator == '/') {

        // Make sure we do not divide by zero.
        if (number2 == 0) {
            strcpy(response, "Error: cannot divide by zero.");
        }

        else {
            sprintf(response, "%g", number1 / number2);
        }
    }

    // Handle an unsupported operator.
    else {
        strcpy(response, "Invalid operator.");
    }
}

// Prints the stored conversation history.
void print_history(struct ConversationTurn history[],
                   int history_count) {

    int i;

    if (history_count == 0) {
        printf("No conversation history yet.\n");
        return;
    }

    printf("\nConversation History:\n");

    for (i = 0; i < history_count; i++) {

        printf("\nTurn %d\n", i + 1);
        printf("You: %s\n", history[i].user_message);
        printf("Assistant: %s\n",
               history[i].assistant_response);
    }

    printf("\n");
}

// Adds one turn to conversation history.
void add_to_history(struct ConversationTurn history[],
                    int *history_count,
                    char user_message[],
                    char assistant_response[]) {

    int i;

    char *new_user_message;
    char *new_assistant_response;

    // If 5 turns are already stored,
    // remove the oldest turn first.
    if (*history_count == MAX_HISTORY) {

        free(history[0].user_message);
        free(history[0].assistant_response);

        // Shift the remaining turns to the left.
        for (i = 0; i < MAX_HISTORY - 1; i++) {
            history[i] = history[i + 1];
        }

        *history_count = MAX_HISTORY - 1;
    }

    // Allocate memory for the new messages.
    new_user_message = malloc(strlen(user_message) + 1);

    new_assistant_response =
        malloc(strlen(assistant_response) + 1);

    // Check if malloc failed.
    if (new_user_message == NULL ||
        new_assistant_response == NULL) {

        free(new_user_message);
        free(new_assistant_response);

        printf("Memory allocation failed.\n");
        return;
    }

    // Copy the messages into the new memory.
    strcpy(new_user_message, user_message);
    strcpy(new_assistant_response, assistant_response);

    // Store the pointers in history.
    history[*history_count].user_message =
        new_user_message;

    history[*history_count].assistant_response =
        new_assistant_response;

    (*history_count)++;
}

int main() {

    char input[256];
    char response[256];

    struct ConversationTurn history[MAX_HISTORY];

    int history_count = 0;
    int i;

    while (1) {

        printf("You: ");

        // Read user input.
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove the newline from fgets().
        input[strcspn(input, "\n")] = '\0';

        // Exit command.
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // History command.
        // This command is not saved in conversation history.
        if (strcmp(input, "history") == 0) {

            print_history(history, history_count);
            continue;
        }

        // Check if the input starts with "calc ".
        if (strncmp(input, "calc ", 5) == 0) {

            // Send calculator requests to calculator().
            calculator(input, response);
        }

        else {

            // Send normal messages to mock_model().
            mock_model(input, response);
        }

        // Print whichever response was created.
        printf("Assistant: %s\n", response);

        // Save the user message and response.
        add_to_history(history,
                       &history_count,
                       input,
                       response);
    }

    // Free all remaining conversation memory.
    for (i = 0; i < history_count; i++) {

        free(history[i].user_message);
        free(history[i].assistant_response);
    }

    printf("Goodbye!\n");

    return 0;
}