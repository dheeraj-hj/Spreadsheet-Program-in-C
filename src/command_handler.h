#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "spreadsheet.h"

void parse_command(spreadsheet *sheet, const char *command); // Parse the command 
void execute_command(); // Execute the command
void validate_command(); // Validate the command
void error_message(int error_code); // Print error message
void number_assign(int *row , int *col, char *expr);
void value_assign(int *row , int *col, char *expr);
void operator_assign(int *row , int *col, char *expr);
void min_handling(int *row , int *col, char *expr);
void max_handling(int *row , int *col, char *expr);
void avg_handling(int *row , int *col, char *expr);
void sum_handling(int *row , int *col, char *expr);
void stdev_handling(int *row , int *col, char *expr);
void sleep_handling(int *row , int *col, char *expr);
void handle_control_command(char control,spreadsheet *sheet); 

#endif
