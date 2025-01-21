#ifndef SPREADSHEET_H
#define SPREADSHEET_H
#include "stdio.h"

// Single Cell Structure in the Spreadsheet
typedef struct{
    char *val; // Value or dependent formula of the cell
    int dependency; // 0 or 1, 1 if the cell has a dependent formula
}cell;

// Spreadsheet Bounds Structure
typedef struct {
    int *first_row; // First row of the spreadsheet
    int *first_col; // First column of the spreadsheet
    int *last_row; // Last row of the spreadsheet
    int *last_col; // Last column of the spreadsheet
}spreadsheetbounds;

// Spreadsheet Structure
typedef struct{
    cell **table; // 2D array of cells
    int rows; // Number of rows in the spreadsheet
    int cols; // Number of columns in the spreadsheet
    spreadsheetbounds *bounds; // Bounds of the spreadsheet
}spreadsheet;

spreadsheet *create_spreadsheet(int rows, int cols); // Create a new spreadsheet
void free_spreadsheet(spreadsheet *sheet); // Free the memory allocated for the spreadsheet
void set_cell(spreadsheet *sheet, int row, int col, char *val); // Set the value of a cell
int evaluate_cell(spreadsheet *sheet, int row, int col); // Evaluate the value of a cell

#endif
