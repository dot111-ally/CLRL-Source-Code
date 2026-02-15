#ifndef CLRL_RUNTIME_H
#define CLRL_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Variable type enumeration
 * 
 * VAR_LARNUM: Arbitrary-precision integer type (no overflow)
 * VAR_PRECISE: Precise decimal type (no floating-point error)
 */
typedef enum {
    VAR_LARNUM,   // Arbitrary-precision integer
    VAR_PRECISE   // Precise decimal number
} VarType;

/**
 * @brief Structure for arbitrary-precision integers (larnum)
 * 
 * Store large integers as strings to avoid overflow issues
 */
typedef struct {
    char* value;  // String representation of large integer (e.g., "12345678901234567890")
} Larnum;

/**
 * @brief Structure for precise decimal numbers (precise)
 * 
 * Split into integer and decimal parts to avoid floating-point errors
 */
typedef struct {
    char* integer_part;  // Integer part (e.g., "0" for 0.123)
    char* decimal_part;  // Decimal part (e.g., "123" for 0.123)
} Precise;

/**
 * @brief Linked list node structure for storing variables
 * 
 * Each node represents a defined variable in the CLRL runtime
 */
typedef struct VarNode {
    char* name;          // Variable name (e.g., "a", "big_num")
    VarType type;        // Variable type (VAR_LARNUM / VAR_PRECISE)
    union {
        Larnum larnum_val;    // Value for larnum type variable
        Precise precise_val;  // Value for precise type variable
    } value;             // Union to save memory for different variable types
    struct VarNode* next; // Pointer to next variable node in linked list
} VarNode;

/**
 * @brief Global variable list head node
 * 
 * All defined variables are stored in this linked list
 */
extern VarNode* var_list;

// ===================== Function Declarations =====================

/**
 * @brief Find a variable by name in the global variable list
 * 
 * @param var_name Name of the variable to find (non-NULL)
 * @return VarNode* Pointer to found variable node (NULL if not defined)
 */
VarNode* find_variable(const char* var_name);

/**
 * @brief Free memory allocated for a single variable node
 * 
 * Prevents memory leaks when redefining or clearing variables
 * 
 * @param node Pointer to the variable node to free (can be NULL)
 */
void free_variable(VarNode* node);

/**
 * @brief Define or redefine a variable in CLRL runtime
 * 
 * If variable exists: free old value and overwrite with new value
 * If variable does not exist: create new node and add to linked list
 * 
 * @param var_name Name of the variable to define (non-NULL)
 * @param type Type of the variable (VAR_LARNUM / VAR_PRECISE)
 * @param new_value Pointer to the new value of the variable (non-NULL)
 * @return int 0 on success, -1 on error
 */
int define_variable(const char* var_name, VarType type, void* new_value);

/**
 * @brief Parse variable definition commands from user input (REPL mode)
 * 
 * Supports syntax: 
 * - larnum <var_name> = <value>
 * - precise <var_name> = <value>
 * 
 * @param input User input string from REPL (non-NULL)
 */
void parse_variable_definition(const char* input);

/**
 * @brief Execute return command to display variable values
 * 
 * Supports syntax: return <var1>,<var2>,...
 * 
 * @param input User input string containing return command (non-NULL)
 */
void execute_return_command(const char* input);

/**
 * @brief Clear all defined variables (called by system:clear)
 * 
 * Frees memory for all variable nodes and resets global list
 */
void clear_all_variables();

/**
 * @brief Free memory for all variables (called on program exit)
 * 
 * Prevents memory leaks when CLRL interpreter exits
 */
void free_all_variables();

/**
 * @brief Calculate Levenshtein distance between two strings (for spelling suggestion)
 * 
 * @param s1 First string (user input)
 * @param s2 Second string (valid command)
 * @return int Edit distance (0 = identical, lower = more similar)
 */
int levenshtein_distance(const char* s1, const char* s2);

/**
 * @brief Find the most similar valid system command for spelling suggestion
 * 
 * @param input User input string (invalid command)
 * @return char* Most similar command (NULL if no match found)
 */
char* find_similar_command(const char* input);

#endif // CLRL_RUNTIME_H