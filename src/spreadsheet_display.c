#include "spreadsheet.h"
#include "utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"


void display_spreadsheet(spreadsheet *sheet){ 
/*
    Function to display the spreadsheet
    - prints 10 rows and 10 columns of the spreadsheet whose bound is defined by sheet->bounds
*/
    int col_width=10;
    
    printf("\t");
    for (int i =*(sheet->bounds->first_col)-1; i<=*(sheet->bounds->last_col)-1 ; i++){
        char* col_label=colIndex_to_name(i);
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

void display_status(const char *status, int time) {
/*
    Function to display the status of the command
    - prints the status of the command and the time taken to execute the command
*/
    printf("[%.1f] (%s) > ", (float)time , status);
}
