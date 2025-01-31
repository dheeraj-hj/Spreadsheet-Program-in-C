#include "spreadsheet.h"
#include "utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"

// Function to display the spreadsheet
void display_spreadsheet(spreadsheet *sheet){ // Display the spreadsheet
// the sheet have these pointers: bounds ,table , rows , cols
// bounds have pointers : first_row , first_col , last_row , last_col
// this function should print :
// first line : tab  first_col_name (A , B ,C ) tab  second_col_name and last_col
// second line : first row_name (1,2,3)  tab value of cell(r1,c1) and so on
    int col_width=10;
    
    printf("\t");
    for (int i =*(sheet->bounds->first_col)-1; i<=*(sheet->bounds->last_col)-1 ; i++){
        char* col_label=colIndex_to_name(i);//index i is passed starts from 0
        printf("%-*s\t",col_width, col_label);
    }
    printf("\n");
    for(int i= *(sheet->bounds->first_row)-1;i<=*(sheet->bounds->last_row)-1;i++){
        printf("%d\t",i+1);
        for (int j =*(sheet->bounds->first_col)-1; j<=*(sheet->bounds->last_col) -1; j++){
            printf("%-*d\t",col_width, sheet->table[i][j].val);
        }
        printf("\n");
    }
}
// Function to display the status of the last operation
void display_status(const char *status, int time) {
    // Display the status and the time it took to run the last command
    printf("[%.1f] (%s) > \n", (float)time , status);
}
