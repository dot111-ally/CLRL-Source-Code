#include "../../include/clrl/clrl_runtime.h"
#include <math.h>
#include <string.h>


// Global variable list initialization (empty by default)
VarNode* var_list = NULL;

/**
 * @brief Safe string copy function (replaces strncpy/strlcpy)
 * No truncation warning + automatic null termination
 */
static void strcopy(char* dest, const char* src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) return;
    // Copy up to dest_size-1 characters
    size_t i = 0;
    while (i < dest_size - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    // Force null termination (critical to avoid warning)
    dest[i] = '\0';
}


/**
 * @brief Find a variable by name in the global variable list
 */
VarNode* find_variable(const char* var_name) {
    if (var_name == NULL || strlen(var_name) == 0) {
        fprintf(stderr, "Error: Invalid variable name (empty or NULL)\n");
        return NULL;
    }

    VarNode* current = var_list;
    while (current != NULL) {
        if (strcmp(current->name, var_name) == 0) {
            return current; // Variable found
        }
        current = current->next;
    }
    return NULL; // Variable not defined
}

/**
 * @brief Free memory allocated for a single variable node
 * Fixed: Typo (type → node->type) + NULL pointer after free
 */
void free_variable(VarNode* node) {
    if (node == NULL) return;

    // Free variable name (clear pointer to avoid wild pointer)
    if (node->name != NULL) {
        free(node->name);
        node->name = NULL;
    }

    // Free value memory based on variable type (FIX: node->type instead of type)
    if (node->type == VAR_LARNUM) {
        if (node->value.larnum_val.value != NULL) {
            free(node->value.larnum_val.value);
            node->value.larnum_val.value = NULL;
        }
    } else if (node->type == VAR_PRECISE) { // 修复：把type改成node->type
        if (node->value.precise_val.integer_part != NULL) {
            free(node->value.precise_val.integer_part);
            node->value.precise_val.integer_part = NULL;
        }
        if (node->value.precise_val.decimal_part != NULL) {
            free(node->value.precise_val.decimal_part);
            node->value.precise_val.decimal_part = NULL;
        }
    }
}

/**
 * @brief Define or redefine a variable in CLRL runtime
 * Fixed: Precise variable redefinition memory allocation + error handling
 */
int define_variable(const char* var_name, VarType type, void* new_value) {
    // Strict input validation
    if (var_name == NULL || strlen(var_name) == 0 || new_value == NULL) {
        fprintf(stderr, "Error: Invalid variable parameters (empty/NULL)\n");
        return -1;
    }

    VarNode* existing_var = find_variable(var_name);

    if (existing_var != NULL) {
        // Case 1: Redefine existing variable (safe free + reassign)
        free_variable(existing_var);
        existing_var->type = type;

        if (type == VAR_LARNUM) {
            Larnum* new_larnum = (Larnum*)new_value;
            existing_var->value.larnum_val.value = strdup(new_larnum->value);
            if (existing_var->value.larnum_val.value == NULL) {
                fprintf(stderr, "Error: Memory allocation failed for larnum '%s'\n", var_name);
                return -1;
            }
        } else if (type == VAR_PRECISE) {
            Precise* new_precise = (Precise*)new_value;
            // Safe strdup with NULL check (fix precise redefinition crash)
            existing_var->value.precise_val.integer_part = strdup(new_precise->integer_part);
            existing_var->value.precise_val.decimal_part = strdup(new_precise->decimal_part);
            
            // Check memory allocation success
            if (existing_var->value.precise_val.integer_part == NULL || 
                existing_var->value.precise_val.decimal_part == NULL) {
                fprintf(stderr, "Error: Memory allocation failed for precise '%s'\n", var_name);
                // Rollback: free allocated memory to avoid leak
                free(existing_var->value.precise_val.integer_part);
                free(existing_var->value.precise_val.decimal_part);
                return -1;
            }
        }

        return 0;
    }

    // Case 2: Create new variable
    VarNode* new_var = (VarNode*)malloc(sizeof(VarNode));
    if (new_var == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for new variable '%s'\n", var_name);
        return -1;
    }

    // Initialize new variable node
    new_var->name = strdup(var_name);
    new_var->type = type;
    new_var->next = var_list;
    var_list = new_var;

    // Assign value (same safe allocation as redefinition)
    if (type == VAR_LARNUM) {
        Larnum* new_larnum = (Larnum*)new_value;
        new_var->value.larnum_val.value = strdup(new_larnum->value);
        if (new_var->value.larnum_val.value == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for larnum '%s'\n", var_name);
            free(new_var->name);
            free(new_var);
            return -1;
        }
    } else if (type == VAR_PRECISE) {
        Precise* new_precise = (Precise*)new_value;
        new_var->value.precise_val.integer_part = strdup(new_precise->integer_part);
        new_var->value.precise_val.decimal_part = strdup(new_precise->decimal_part);

        if (new_var->value.precise_val.integer_part == NULL || 
            new_var->value.precise_val.decimal_part == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for precise '%s'\n", var_name);
            // Rollback all allocated memory
            free(new_var->name);
            free(new_var->value.precise_val.integer_part);
            free(new_var->value.precise_val.decimal_part);
            free(new_var);
            return -1;
        }
    }
    return 0;
}

/**
 * @brief Parse variable definition commands from user input (REPL mode)
 */
void parse_variable_definition(const char* input) {
    if (input == NULL) {
        fprintf(stderr, "Error: Input string is NULL\n");
        return;
    }

    char var_name[256] = {0};
    char value_str[1024] = {0};
    VarType type;
    char trimmed_input[2048] = {0};

    // Remove extra whitespace from input
    int i = 0, j = 0;
    while (input[i] != '\0') {
        if (!isspace(input[i]) || (j > 0 && !isspace(trimmed_input[j-1]))) {
            trimmed_input[j++] = input[i];
        }
        i++;
    }
    trimmed_input[j] = '\0';

    // Parse larnum variable definition
    if (sscanf(trimmed_input, "larnum%[^=]=%s", var_name, value_str) == 2) {
        type = VAR_LARNUM;
        Larnum new_larnum;
        new_larnum.value = strdup(value_str);
        
        if (new_larnum.value == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for larnum value\n");
            return;
        }

        define_variable(var_name, type, &new_larnum);
        free(new_larnum.value); // Free temporary value memory
    }
    // Parse precise variable definition
    else if (sscanf(trimmed_input, "precise%[^=]=%s", var_name, value_str) == 2) {
        type = VAR_PRECISE;
        Precise new_precise;
        
        // Split value into integer and decimal parts
        char* dot_pos = strchr(value_str, '.');
        if (dot_pos != NULL) {
            *dot_pos = '\0';
            new_precise.integer_part = strdup(value_str);
            new_precise.decimal_part = strdup(dot_pos + 1);
        } else {
            // No decimal point - set decimal part to empty string
            new_precise.integer_part = strdup(value_str);
            new_precise.decimal_part = strdup("");
        }

        if (new_precise.integer_part == NULL || new_precise.decimal_part == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for precise value\n");
            free(new_precise.integer_part);
            free(new_precise.decimal_part);
            return;
        }

        define_variable(var_name, type, &new_precise);
        // Free temporary value memory
        free(new_precise.integer_part);
        free(new_precise.decimal_part);
    }
    // Invalid syntax
    else {
        fprintf(stderr, "Error: Invalid variable definition syntax\n");
        fprintf(stderr, "Usage: \n");
        fprintf(stderr, "  larnum <var_name> = <integer_value>\n");
        fprintf(stderr, "  precise <var_name> = <decimal_value>\n");
    }
}

/**
 * @brief Execute return command to display variable values
 * Fixed: Null pointer check + safe string handling (fix return a crash)
 */
void execute_return_command(const char* input) {
    if (input == NULL || strlen(input) == 0) {
        fprintf(stderr, "Error: Empty input for return command\n");
        return;
    }

    char var_list_str[2048] = {0}; // Initialize to 0 to avoid dirty data
    // Limit input length to prevent buffer overflow
    if (sscanf(input, "return %2000[^\n]", var_list_str) != 1) {
        fprintf(stderr, "Error: Invalid return syntax (usage: return <var_name>)\n");
        return;
    }

    // Check if no variable name is provided
    if (strlen(var_list_str) == 0) {
        fprintf(stderr, "Error: No variable specified (e.g., 'return a')\n");
        return;
    }

    // Safe split variable names (use temp buffer to avoid modifying original string)
    char temp_buf[2048] = {0};
    // Fixed: strncpy truncation warning (manual null terminator)
    strcopy(temp_buf, var_list_str, sizeof(temp_buf));
    temp_buf[sizeof(temp_buf) - 1] = '\0'; // Critical: ensure null termination
    char* var_name = strtok(temp_buf, ",");

    // Limit max variables to prevent infinite loop
    int max_vars = 100;
    int var_count = 0;

    while (var_name != NULL && var_count < max_vars) {
        var_count++;

        // Trim whitespace from variable name (fix "return  a" or "return a ")
        char trimmed_name[256] = {0};
        int i = 0, j = 0;
        while (var_name[i] != '\0' && j < 255) {
            if (!isspace(var_name[i])) {
                trimmed_name[j++] = var_name[i];
            }
            i++;
        }

        // Skip empty trimmed names (e.g., multiple commas: return a,,b)
        if (strlen(trimmed_name) == 0) {
            var_name = strtok(NULL, ",");
            continue;
        }

        // Find variable (critical: check if var is NULL before accessing)
        VarNode* var = find_variable(trimmed_name);
        if (var == NULL) {
            fprintf(stderr, "Error: Variable '%s' is not defined\n", trimmed_name);
            var_name = strtok(NULL, ",");
            continue;
        }

        // Safe print variable value (no null pointer access)
        printf("%s: ", trimmed_name);
        if (var->type == VAR_LARNUM) {
            printf("%s\n", (var->value.larnum_val.value != NULL) ? var->value.larnum_val.value : "");
        } else if (var->type == VAR_PRECISE) {
            // Check if integer/decimal part is NULL (fix precise variable crash)
            char* int_part = (var->value.precise_val.integer_part != NULL) ? var->value.precise_val.integer_part : "";
            char* dec_part = (var->value.precise_val.decimal_part != NULL) ? var->value.precise_val.decimal_part : "";
            printf("%s.%s\n", int_part, dec_part);
        }

        var_name = strtok(NULL, ",");
    }

    // Prevent infinite loop if max variables reached
    if (var_count >= max_vars) {
        fprintf(stderr, "Warning: Maximum number of variables (100) processed\n");
    }
}

/**
 * @brief Clear all defined variables (called by system:clear)
 */
void clear_all_variables() {
    VarNode* current = var_list;
    while (current != NULL) {
        VarNode* next = current->next;
        free_variable(current);
        free(current);
        current = next;
    }
    var_list = NULL;
    printf("Success: All variables cleared\n");
}

/**
 * @brief Free memory for all variables (called on program exit)
 */
void free_all_variables() {
    clear_all_variables(); // Reuse clear logic to free all nodes
}
/**
 * @brief Calculate Levenshtein distance between two strings
 * 
 * Implementation of Levenshtein distance algorithm to measure string similarity
 */
int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int matrix[len1 + 1][len2 + 1];

    // Initialize matrix
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;

    // Calculate edit distance
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            matrix[i][j] = fmin(fmin(matrix[i-1][j] + 1,    // Deletion
                                      matrix[i][j-1] + 1),  // Insertion
                                      matrix[i-1][j-1] + cost); // Substitution
        }
    }

    return matrix[len1][len2];
}

/**
 * @brief Find the most similar valid system command
 */
char* find_similar_command(const char* input) {
    if (input == NULL || strlen(input) == 0) return NULL;

    // List of valid system commands (extend this list as needed)
    const char* valid_commands[] = {
        "system:help",
        "system:clear",
        "system:exit",
        "system:fast",
        "system:compile",
        "system:version"
    };
    int num_commands = sizeof(valid_commands) / sizeof(valid_commands[0]);

    // Find command with minimum edit distance (max 3 edits for valid suggestion)
    int min_distance = 4; // Ignore matches with >3 edits (too dissimilar)
    char* best_match = NULL;

    for (int i = 0; i < num_commands; i++) {
        int distance = levenshtein_distance(input, valid_commands[i]);
        if (distance < min_distance) {
            min_distance = distance;
            best_match = (char*)valid_commands[i];
        }
    }

    return best_match;
}