#include <stdio.h>
#include <string.h>
#include "../../include/clrl/clrl_runtime.h"

#define MAX_INPUT_LENGTH 2048

/**
 * @brief Handle system commands (system:fast, system:compile, etc.)
 * 
 * @param input User input string containing system command
 * @return int 1 if command handled, 0 if not a system command, -1 if typo detected
 */
int handle_system_commands(const char* input) {
    if (strstr(input, "system:clear") != NULL) {
        clear_all_variables();
        return 1;
    } else if (strstr(input, "system:exit") != NULL) {
        free_all_variables();
        printf("Exiting CLRL REPL...\n");
        exit(0);
    } else if (strstr(input, "system:help") != NULL) {
        printf("CLRL REPL Help:\n");
        printf("  - Define variables: larnum <name>=<value> | precise <name>=<value>\n");
        printf("  - Return values: return <var1>,<var2>,...\n");
        printf("  - System commands: system:clear | system:exit | system:help\n");
        printf("  - Redefine variables: Just re-define (e.g., larnum a=10 → larnum a=20)\n");
        return 1;
    } else if (strstr(input, "system:fast") != NULL) {
        printf("Executing in fast mode...\n");
        return 1;
    } else if (strstr(input, "system:compile") != NULL) {
        printf("Compiling and executing...\n");
        return 1;
    } else if (strstr(input, "system:version") != NULL) {
        printf("CLRL v1.0.0 (MinGW GCC 15.2.0)\n");
        return 1;
    } else if (strstr(input, "system:") != NULL) {
        // Detected system: prefix but invalid command (typo)
        return -1;
    }
    return 0;
}

/**
 * @brief Main REPL loop for CLRL interpreter
 */
void repl_loop() {
    char input[MAX_INPUT_LENGTH];
    int command_count = 0;

    printf("CLRL Calculator Language REPL (v1.0.0)\n");
    printf("Type 'system:help' for usage information\n");

    while (1) {
        // Print prompt with command number
        printf("clrl> (%d) ", ++command_count);
        fflush(stdout);

        // Read user input
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
            continue;
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Skip empty input
        if (strlen(input) == 0) {
            command_count--;
            continue;
        }

        // Handle system commands first
        if (handle_system_commands(input)) {
            continue;
        }

        // Handle return command
        if (strstr(input, "return") != NULL) {
            execute_return_command(input);
            continue;
        }

        // Handle variable definition/redefinition
        if (strstr(input, "larnum") != NULL || strstr(input, "precise") != NULL) {
            parse_variable_definition(input);
            continue;
        }

        // Invalid command
        fprintf(stderr, "Error: Invalid command - type 'system:help' for valid syntax\n");
    }
}

/**
 * @brief Main function of CLRL interpreter
 */
#define UNUSED(x) (void)(x)
int main(int argc, char* argv[]) {
    // REPL mode (no arguments)
    UNUSED(argv);
    if (argc == 1) {
        repl_loop();
    } 
    // File execution mode (with arguments)
    else {
        fprintf(stderr, "File execution mode not implemented yet\n");
        return 1;
    }

    // Cleanup on exit
    free_all_variables();
    return 0;
}