#ifndef SPREADSHEET_H
#define SPREADSHEET_H

// Single Cell Structure in the Spreadsheet
typedef struct{
    char *val; // Value or dependent formula of the cell
    int dependency; // 0 or 1, 1 if the cell has a dependent formula
}cell;


// Spreadsheet Structure
typedef struct{
    cell **table; // 2D array of cells
    int rows; // Number of rows in the spreadsheet
    int cols; // Number of columns in the spreadsheet
}spreadsheet;

spreadsheet *create_spreadsheet(int rows, int cols); // Create a new spreadsheet
void free_spreadsheet(spreadsheet *sheet); // Free the memory allocated for the spreadsheet
void set_cell(spreadsheet *sheet, int row, int col, char *val); // Set the value of a cell
int evaluate_cell(spreadsheet *sheet, int row, int col); // Evaluate the value of a cell

#endif