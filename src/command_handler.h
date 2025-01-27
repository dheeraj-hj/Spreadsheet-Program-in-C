#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "spreadsheet.h"

void parse_command(spreadsheet *sheet, const char *command); // Parse the command 
void execute_command(); // Execute the command
void validate_command(spreadsheet* sheet, const char *command , char *targetcell , char *expression , int *error_code); // Validate the command
void error_message(int error_code); // Print error message
void number_assign(spreadsheet* sheet, int *row , int *col, const char *expr);
void value_assign(spreadsheet* sheet, int *row , int *col, const char *expr);
void operator_assign(spreadsheet* sheet, int *row , int *col,const char *expr);
void min_handling(spreadsheet* sheet , int *row , int *col ,const char *expr);
void max_handling(spreadsheet* sheet , int *row, int *col , const char *_expr);
void avg_handling(spreadsheet* sheet , int *row, int *col , const char *_expr);
void sum_handling(spreadsheet* sheet , int *row , int *col , const char *_expr);
void stdev_handling(int *row , int *col, char *expr);
void sleep_handling(int *row , int *col, char *expr);
void handle_control_command(char control,spreadsheet *sheet); 

#endif
