#ifndef SPREADSHEET_H
#define SPREADSHEET_H
#include "stdio.h"

typedef struct {
    int* data;
    size_t size;
    size_t capacity;
} IntArray;

// Single Cell Structure in the Spreadsheet
typedef struct{
    int val;               // Value of the cell
    const char *formula;         // Formula or dependent value of the cell
    int vis;        // will be using for recalculating the cell
    IntArray parents; // Array of parent cell hash values
    IntArray children; // Array of children cell hash values
    char error;
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

typedef struct Stack {
    int top;
    int capacity;
    int* array;
}Stack;

extern int tester;
spreadsheet *create_spreadsheet(int rows, int cols); // Create a new spreadsheet
void free_int_array(IntArray *arr);
void free_cell(cell *c);
void free_spreadsheet(spreadsheet *sheet); // Free the memory allocated for the spreadsheet
void evaluate_cell(spreadsheet *sheet, int row, int col); // Evaluate the value of a cell
Stack* createStack(int capacity);
int isEmpty(Stack* stack);
void resizeStack(Stack* stack);
void push(Stack* stack, int item);
int pop(Stack* stack);
void init_int_array(IntArray* arr, size_t initial);
void add_element(IntArray* arr, int value);

#endif