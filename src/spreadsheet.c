#include "spreadsheet.h"
#include "utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"

spreadsheet *create_spreadsheet(int rows, int cols){
    spreadsheet *s = (spreadsheet *)malloc(sizeof(spreadsheet));
    s->rows = rows;
    s->cols = cols;
    s->table = (cell **)malloc(rows * sizeof(cell *));
    for(int i = 0; i < rows; i++){
        s->table[i] = (cell *)malloc(cols * sizeof(cell));
        for(int j = 0; j < cols; j++){
            s->table[i][j].val = NULL;
            s->table[i][j].dependency = 0;
        }
    }
    return s;
}

void set_cell(spreadsheet *sheet, int row, int col, char *val){
    if(sheet->table[row][col].val != NULL){
        free(sheet->table[row][col].val);
    }
    sheet->table[row][col].val = (char *)malloc(strlen(val) + 1);
    strcpy(sheet->table[row][col].val, val);
    if(is_number(val)){
        sheet->table[row][col].dependency = 0;
    }else{
        sheet->table[row][col].dependency = 1;
    }
}