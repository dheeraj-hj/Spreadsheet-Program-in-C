#ifndef UTILS_H
#define UTILS_H
#define min(a,b) (a<b?a:b)
#define max(a,b) (a>b?a:b)

#include "spreadsheet.h"

void name_to_indices(const char *name, int *row, int *col); // Convert the cell name to row and column indices
void range_to_indices(const char *range, int *row1, int *col1, int *row2, int *col2); // Convert the range to row and column indices
char* trim_space(char *str); // Trim the leading and trailing spaces from a string
int is_number(const char *val); // Check if a string is a number
int valid_cell(spreadsheet* sheet , char *cell , int *row_id , int *col_id);
int valid_range(spreadsheet *sheet , char *range , int *error_code);
int valid_expression(spreadsheet* sheet, char *expression , int *expr_type , int *error_code);
// Still more functions to be added

#endif