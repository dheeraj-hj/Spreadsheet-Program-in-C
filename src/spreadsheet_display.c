#include "spreadsheet.h"
#include "utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"

// Function to display the spreadsheet
void display_sheet(spreadsheet *sheet) {
    printf("\t");
    for (int col = 0; col < sheet->cols; col++) {
        char col_label = 'A' + col;
        printf("%c\t", col_label);
    }
    printf("\n");

    // Display rows
    for (int row = 0; row < sheet->rows; row++) {
        printf("%d\t", row + 1); // Display row number
        for (int col = 0; col < sheet->cols; col++) {
            printf("%d\t", sheet->table[row][col]);
        }
        printf("\n");
    }
}

// Function to display the status of the last operation
void display_status(const char *status, int time) {
    // Display the status and the time it took to run the last command
    printf("[%.1f] (%s) > ", (float)time / 1000, status);
}
