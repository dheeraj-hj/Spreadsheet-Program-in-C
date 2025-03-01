#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "spreadsheet.h"

extern int tester_mode;

void parse_command(spreadsheet *sheet, const char *command); // Parse the command 
void execute_command(); // Execute the command
void validate_command(spreadsheet* sheet, const char *cmd , int *error_code); // Validate the command
void error_message(int error_code , int time); // Print error message
void number_assign(spreadsheet* sheet, int *row , int *col, const char *expr);
void value_assign(spreadsheet* sheet, int *row , int *col, const char *expr , int* error_code);
void operator_assign(spreadsheet* sheet, int *row , int *col,const char *expr , int* error_code);
void min_handling(spreadsheet* sheet , int *row , int *col ,const char *expr , int* error_code);
void max_handling(spreadsheet* sheet , int *row, int *col , const char *_expr , int* error_code);
void avg_handling(spreadsheet* sheet , int *row, int *col , const char *_expr , int* error_code);
void sum_handling(spreadsheet* sheet , int *row , int *col , const char *_expr , int* error_code);
void stdev_handling(spreadsheet* sheet,int *row , int *col, const char *expr , int* error_code);
void sleep_handling(spreadsheet* sheet,int *row , int *col,const char *expr , int* error_code);
void handle_control_command(char control,spreadsheet *sheet); 
void scroll_to(spreadsheet *sheet, const char *cell , int *error_code);

#endif
