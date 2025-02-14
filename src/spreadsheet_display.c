#include "spreadsheet.h"
#include "utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"



void display_spreadsheet(spreadsheet *sheet){ 

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
            if(sheet->table[i][j].error == '0'){
                printf("%-*d\t",col_width, sheet->table[i][j].val);
            }else{
                printf("%-*s\t",col_width, "ERR");
            }
        }
        printf("\n");
    }
    
    // Existing code...

    // Print children and parents
    // for(int i= *(sheet->bounds->first_row)-1;i<=*(sheet->bounds->last_row)-1;i++){
    //     for (int j =*(sheet->bounds->first_col)-1; j<=*(sheet->bounds->last_col) -1; j++){
    //         printf("Node [%d, %d]:\n", i+1, j+1);
            
    //         // Print children
    //         printf("Children: ");
    //         for (int k = 0; k < sheet->table[i][j].children.size; k++) {
    //             printf("%d ", sheet->table[i][j].children.data[k]);
    //         }
    //         printf("\n");
            
    //         // Print parents
    //         printf("Parents: ");
    //         for (int k = 0; k < sheet->table[i][j].parents.size; k++) {
    //             printf("%d ", sheet->table[i][j].parents.data[k]);
    //         }
    //         printf("\n");
    //     }
    // }

    // - prints 10 rows and 10 columns of the spreadsheet whose bound is defined by sheet->bounds
}

void display_status(const char *status,  double time) {
/*
    Function to display the status of the command
    - prints the status of the command and the time taken to execute the command
*/
    printf("[%.1f] (%s) > ", time , status);
}