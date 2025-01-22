#include "utils.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"

void range_to_indices(const char *range, int *row1, int *col1, int *row2, int *col2){
    char start_cell[10] ={0} ;
    char end_cell[10] ={0};
    int i =0,j  = 0 ;

    while (isalpha(range[i])  || isdigit(range[i])) {
        start_cell[j++] =  range[i++]  ;
    }
    start_cell[j] ='\0';

    if (range[i] == ':') {
        i++ ;
    } else {
        *row1 = *col1  = *row2 = *col2 =-1 ;
        return ;
    }

    j= 0 ;
    while (isalpha(range[i]) || isdigit(range[i])) {
        end_cell[j++] = range[i++];
    }
    end_cell[j] ='\0' ;

    name_to_indices(start_cell, row1, col1);
    name_to_indices(end_cell, row2, col2) ;

        // just checking if index are positive or not.
        if (*row1 <0 || *col1 <0 || *row2< 0 || *col2 < 0) {
            // Handle invalid indices
            *row1 = *col1 = *row2 = *col2 =-1 ;
        }
    return ;
}

void name_to_indices(const char *name, int *row, int *col){
    *col=0;
    *row=0;
    int i= 0;
    while (isalpha(name[i])) {
        *col= *col * 26 +(name[i] -'A' +1)  ;
        i++ ;
    }
    
    while(isdigit(name[i])) {
        *row=  *row*10 + (name[i]- '0');
        i++ ;
    }
    *row-=1;
    *col-=1;
}

char* colIndex_to_name(int i){
    // function assumses that i lies between 0 and 18,277
    static char col_name[3];
    int index = 0;

    while (i >=0) {
        col_name[index++] = (i % 26) + 'A';
        i = (i/26)-1;
        if (i< 0) break;
    }

    col_name[index] ='\0';
    
    for (int j = 0, k = index - 1; j < k; j++, k--) {
            char temp = col_name[j];
            col_name[j] = col_name[k];
            col_name[k] = temp;
        }
    
    return col_name;

}

int is_number(const char *val) {
    for (int i = 0; val[i] != '\0'; i++) {
        if (!(val[i] >= '0' && val[i] <= '9')) {
            return 0;
        }
    }
    return 1;
}

int valid_cell(spreadsheet* sheet , char *cell , int *row_id, int *col_id){
    int i = 0;
    while(cell[i] && isalpha(cell[i])){
        i++;
    }
    if(i == 0) return 0;
    char col[i + 1];
    strncpy(col , cell , i);
    col[i] = '\0';

    char *row_str = cell + i;
    int j = 0;
    while(row_str[j]){
        if(!isdigit(row_str[j])){
            return 0;
        }
        j++;
    }
    if(j == 0) return 0;
    *row_id = atoi(row_str);
    *col_id = 0;
    char *temp_col = col;
    while(*temp_col){
        if(*temp_col < 'A' || *temp_col > 'Z'){
            return 0;
        }
        *col_id = *col_id * 26 + (*temp_col - 'A' + 1);
        temp_col++;
    }
    
    if(*col_id > sheet->cols || *row_id > sheet->rows){
        return 0;
    }
    return 1;
}

int valid_range(spreadsheet *sheet , char *range , int *error_code){
    char *colon = strchr(range , ':');
    if(colon){
        *colon = '\0';
        char *start = range;
        char *end = colon + 1;
        int row_topleft , col_topleft , row_bottomright , col_bottomright;
        if(valid_cell(sheet , start , &row_topleft , &col_topleft) && valid_cell(sheet , end , &row_bottomright , &col_bottomright)){
            if(row_topleft <= row_bottomright && col_topleft <= col_bottomright){
                return 1;
            }else{
                *error_code = 1;
                return 0;
            }
        }
    }
    return 0;
}
int valid_expression(spreadsheet* sheet, char *expression , int *expr_type , int *error_code){

/*
    Function will return 0 if invalid expression and 1 if valid expression
    It will also return expression type:
    0 - number
    1 - cell
    2 - val (operator) val
    3 - MIN
    4 - MAX
    5 - AVG
    6 - SUM
    7 - STDEV
    8 - SLEEP(number)
    9 - SLEEP(cell)
*/
    int row_id;
    int col_id;

    if(is_number(expression)){
        *expr_type = 0;
        return 1;
    }
    else if(valid_cell(sheet , expression , &row_id , &col_id)){
        *expr_type = 1;
        return 1;
    }
    else if(strchr(expression, '+') || strchr(expression, '-') || strchr(expression, '*') || strchr(expression, '/')){
        char *operators = "+-*/";
        char *op = strpbrk(expression , operators);
        if(op){
            char *left = expression;
            char *right = op+1;
            *op = '\0';
            if((is_number(left) || valid_cell(sheet , left ,  &row_id , &col_id)) && (is_number(right) || valid_cell(sheet , right, &row_id , &col_id))){
                *expr_type = 2;
                return 1;
            }
        }
    }
    else if((strncmp(expression, "MIN(" , 4) == 0) ){
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 3;
                return 1;
            }
        }
    }
    else if ((strncmp(expression, "MAX(" , 4) == 0) ){
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 4;
                return 1;
            }
        }
    }
    else if ((strncmp(expression, "AVG(" , 4) == 0)) {
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 5;
                return 1;
            }
        }
    }
    else if ((strncmp(expression, "SUM(" , 4) == 0)) {
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 6;
                return 1;
            }
        }
    }
     else if ((strncmp(expression, "STDEV(" , 6) == 0)) {
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 7;
                return 1;
            }
        }
    }
    else if(strncmp(expression , "SLEEP(" , 6) == 0){
        char *range_end = strchr(expression , ')');
        char *fun_end = strchr(expression , '(');
        if(fun_end && range_end && range_end > fun_end){
            char *time = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';

            if(is_number(time)){
                *expr_type = 8;
                return 1;
            }
            else if(valid_cell(sheet , time,  &row_id , &col_id)){
                *expr_type = 9;
                return 1;
            }
            
        }

    }
    return 0;
}
