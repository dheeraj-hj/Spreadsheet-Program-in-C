#ifndef UTILS_H
#define UTILS_H

#include "spreadsheet.h"
char* colIndex_to_name(const int i);
void name_to_indices(const char *name, int *row, int *col); // Convert the cell name to row and column indices
void range_to_indices(const char *range, int *row1, int *col1, int *row2, int *col2); // Convert the range to row and column indices
char* trim_space(const char *str); // Trim the leading and trailing spaces from a string
int is_number(const char *val); // Check if a string is a number
int valid_cell(spreadsheet* sheet ,const char *cell , int *row_id , int *col_id);
int valid_range(spreadsheet *sheet , const char *range , int *error_code);
int valid_expression(spreadsheet* sheet, const char *expression , int *expr_type , int *error_code);
int calculate_max(spreadsheet* sheet, int row1, int col1, int row2, int col2);
int calculate_min(spreadsheet* sheet, int row1, int col1, int row2, int col2);
int calculate_sum(spreadsheet* sheet, int row1, int col1, int row2, int col2);
int calculate_avg(spreadsheet* sheet, int row1, int col1, int row2, int col2);
int calculate_stdev(spreadsheet* sheet, int row1, int col1, int row2, int col2);
void add_child(cell* c , int child_hash);
void add_parent(cell* c , int parent_hash);
int hash_index(spreadsheet *sheet , int row, int col);
int check_cycle(spreadsheet *sheet ,cell *c, int* target_cell_hash);
void delete_parent_connections(spreadsheet *sheet, cell *c , int *row , int *col);
void recalculate_dependent_cells(spreadsheet *sheet , int *row ,int *col);
void dfs(spreadsheet *sheet , int row , int col , Stack *stk);

#endif