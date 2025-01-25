#include "spreadsheet.h"
#include "utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

spreadsheet *create_spreadsheet(int rows, int cols){
    spreadsheet *s = (spreadsheet *)malloc(sizeof(spreadsheet));
    s->rows = rows;
    s->cols = cols;
    s->table = (cell **)malloc(rows * sizeof(cell *));
    s->bounds=(spreadsheetbounds *)malloc(sizeof(spreadsheetbounds));
    for(int i = 0; i < rows; i++){
        s->table[i] = (cell *)malloc(cols * sizeof(cell));
        for(int j = 0; j < cols; j++){
            s->table[i][j].val = 0;
            s->table[i][j].dependency = '0';
            s->table[i][j].formula = NULL;
            s->table[i][j].parents = NULL;
            s->table[i][j].children = NULL;
            s->table[i][j].num_parents = 0;
            s->table[i][j].num_children = 0;
        }
    }
    s->bounds->first_row=(int *)malloc(sizeof(int));
    s->bounds->first_col=(int *)malloc(sizeof(int));
    s->bounds->last_row=(int *)malloc(sizeof(int));
    s->bounds->last_col=(int *)malloc(sizeof(int));
    *(s->bounds->first_row)=1;
    *(s->bounds->first_col)=1;
    *(s->bounds->last_row)=MIN(rows,10);
    *(s->bounds->last_col)=MIN(cols,10);
    return s;
}
    
// void set_cell(spreadsheet *sheet, int row, int col, char *val){
//     if(sheet->table[row][col].val != NULL){
//         free(sheet->table[row][col].val);
//     }
//     sheet->table[row][col].val = (char *)malloc(strlen(val) + 1);
//     strcpy(sheet->table[row][col].val, val);
//     if(is_number(val)){
//         sheet->table[row][col].dependency = 0;
//     }else{
//         sheet->table[row][col].dependency = 1;
//     }
// }